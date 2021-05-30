#include "page_cache.h"
#include "slotted_data_page.h"
#include "page.h"
#include "row_mapping_page.h"
#include "page_creator.h"

#include <utility>
#include <fstream>
#include <iostream>

page::Page *page_cache::PageCache::get_page_id(
    page::PageId page_id,
    const std::string &table_name,
    page::PageType page_type
) {
    auto key = generate_cache_key(page_id, table_name, page_type);
    auto position = _pages.find(key);
    if (position == _pages.end()) {
        handle_missing_cache_entry(page_id, table_name, page_type, key);
        return _page_ids.begin()->second.get();
    } else {
        _page_ids.splice(_page_ids.begin(), _page_ids, position->second);
        return position->second->second.get();
    }
}

void page_cache::PageCache::handle_missing_cache_entry(
    unsigned int page_id,
    const std::string &table_name,
    page::PageType &page_type,
    const std::string &key
) {
    auto page = load_page_from_disk(page_id, table_name, page_type);

    if (_page_ids.size() == _max_size) {
        std::cout << "triggering eviction, max size reached\n";
        evict();
    }

    _page_ids.push_front(std::make_pair(key, std::move(page)));
    _pages.insert({key, _page_ids.begin()});
}

page_cache::PageCache::PageCache(uint32_t max_size, const std::vector<std::string> &tables)
    : _max_size{max_size} {
    for (const auto &table: tables) {
        _table_file_names.insert({table, file_name_from_table_name(table)});
    }

    scan_tables();
}

std::unique_ptr<page::Page>
page_cache::PageCache::load_page_from_disk(page::PageId page_id, const std::string &table_name,
                                           page::PageType page_type) {
    auto table_file_name = _table_file_names[table_name];
    std::ifstream i{table_file_name, std::ios::binary};

    if (!i) {
        throw std::ios::failure("failed to open table file");
    }

    i.seekg(page::k_page_size * page_id, std::ios::beg);
    auto buffer = stream_utils::read_page_from_stream(i);

    if (!i) {
        throw std::ios::failure("failed to read _buffer from disk");
    }

    return make_page_from_buffer(page_type, buffer);
}

std::unique_ptr<page::Page>
page_cache::PageCache::make_page_from_buffer(const page::PageType &page_type,
                                             const stream_utils::ByteBuffer &buffer) {
    switch (page_type) {
        case page::PageType::Data:
            return std::make_unique<page::SlottedDataPage>(buffer);
        case page::PageType::RowMap:
            return std::make_unique<page::RowMappingPage>(buffer);
        case page::PageType::Metadata:
            return std::make_unique<page::MetadataPage>(buffer);
        default:
            throw std::invalid_argument{"Invalid page type!"};
    }
}

void page_cache::PageCache::evict() {
    auto page_id_to_evict = _page_ids.back().first;
    _page_ids.pop_back();
    _pages.erase(page_id_to_evict);
    std::cout << "evicted page id: " << page_id_to_evict << "\n";
}

void page_cache::PageCache::scan_tables() {
    for (const auto&[table, file]: _table_file_names) {
        scan_table_file(table, file);
    }
}

void page_cache::PageCache::scan_table_file(const std::string &table_name, const std::string &file_name) {
    std::ifstream ifs{file_name, std::ios::binary};
    if (!ifs) {
        throw std::ios::failure{"failed to read table file"};
    }
    stream_utils::ByteBuffer buffer(page::k_page_size);

    ifs.read(reinterpret_cast<char *>(buffer.data()), page::k_page_size);

    page::MetadataPage m{buffer};

    scan_free_pages_in_table_stream(table_name, ifs, m.n_marked_pages());
}

std::pair<page::RowMappingPage *, page::SlottedDataPage *>
page_cache::PageCache::get_pages_for_data_size(const std::string &table_name, uint32_t data_size) {
    auto data_page_id_maybe = get_page_id_for_size(table_name, data_size, page::PageType::Data);
    auto row_map_page_id_maybe = get_page_id_for_size(table_name, page::k_record_width, page::PageType::RowMap);
    auto *metadata_page = metadata_page_for_table(table_name);

    page::PageCreator page_creator{table_name, metadata_page};
    uint32_t data_page_id = data_page_id_maybe ? *data_page_id_maybe : page_creator.create_page(page::PageType::Data);
    uint32_t row_map_page_id = row_map_page_id_maybe ? *row_map_page_id_maybe : page_creator.create_page(
        page::PageType::RowMap);

    auto *data_page = dynamic_cast<page::SlottedDataPage *>(get_page_id(data_page_id, table_name,
                                                                        page::PageType::Data));

    auto *row_map_page = dynamic_cast<page::RowMappingPage *>(get_page_id(row_map_page_id, table_name,
                                                                          page::PageType::RowMap));

    auto pos = _dirty_pages.find(table_name);
    if (pos == _dirty_pages.end()) {
        _dirty_pages[table_name] = {{row_map_page, data_page, metadata_page}};
    } else {
        pos->second.insert({row_map_page, data_page, metadata_page});
    }

    return std::make_pair(row_map_page, data_page);
}

uint32_t page_cache::PageCache::row_id_for_table(const std::string &table_name) {
    auto *metadata_page = dynamic_cast<page::MetadataPage *>(get_page_id(0, table_name, page::PageType::Metadata));
    return metadata_page->next_row_id();
}

void page_cache::PageCache::write_dirty_pages(const std::string &table_name) {
    std::fstream ofs{file_name_from_table_name(table_name), std::ios::binary | std::ios::in | std::ios::out};
    if (!ofs) {
        throw std::ios::failure{"failed to read table file"};
    }

    for (const auto &page_ptr: _dirty_pages.at(table_name)) {
        auto page_id = page_ptr->page_id();
        auto buffer = page_ptr->buffer();

        switch (page_ptr->page_type()) {
            case page::PageType::Data: {
                auto base_offset = page::k_page_size;
                base_offset += (page_id * page::k_page_size);
                ofs.seekp(base_offset, std::ios::beg);
                break;
            }
            case page::PageType::RowMap: {
                ofs.seekp(0, std::ios::end);
                uint32_t end = ofs.tellp();

                auto base_offset = end - ((page_id + 1) * page::k_page_size);
                ofs.seekp(base_offset, std::ios::beg);
                break;
            }
            case page::PageType::Metadata: {
                ofs.seekp(0, std::ios::beg);
                break;
            }
        }

        ofs.write(reinterpret_cast<const char *>(buffer.data()), page::k_page_size);
        if (!ofs) {
            throw std::ios::failure{"failed to write page to file"};
        }
    }
}

page::RowMappingPage *page_cache::PageCache::rowmap_page_for_row_id(const std::string &table_name, uint32_t row_id) {
    // TODO get the actual rowmap page...
    return dynamic_cast<page::RowMappingPage *>(get_page_id(0, table_name, page::PageType::RowMap));
}

page::MetadataPage *page_cache::PageCache::metadata_page_for_table(const std::string &table_name) {
    return dynamic_cast<page::MetadataPage *>(get_page_id(0, table_name, page::PageType::Metadata));
}

void page_cache::PageCache::scan_free_pages_in_table_stream(const std::string &table_name, std::istream &is,
                                                            uint32_t n_pages_to_scan) {
    // skip ahead of the metadata page
    is.seekg(page::k_page_size, std::ios::beg);

    std::vector<page::FreePageInfo> free_data_pages{};
    std::vector<page::FreePageInfo> free_row_map_pages{};

    for (auto i = 0; i < n_pages_to_scan; ++i) {
        // TODO - read only the header here. we can get page id and free space from there.
        //  No need to load up the entire buffer in memory.
        //  Then we also need to advance the stream manually to next page offset.
        auto buffer = stream_utils::read_page_from_stream(is);
        auto free_space = page::free_space_from_buffer(buffer);
        if (free_space > 0) {
            switch (page::page_type_from_buffer(buffer)) {
                case page::PageType::Data: {
                    auto data_page = page::SlottedDataPage{buffer};
                    free_data_pages.emplace_back(data_page.page_id(), free_space);
                    break;
                }
                case page::PageType::RowMap: {
                    auto rowmap_page = page::RowMappingPage{buffer};
                    free_row_map_pages.emplace_back(rowmap_page.page_id(), free_space);
                    break;
                }
                case page::PageType::Metadata:
                    break;
            }
        }
    }

    _free_data_pages.insert({table_name, free_data_pages});
    _free_rowmap_pages.insert({table_name, free_row_map_pages});
}

std::optional<page::PageId>
page_cache::PageCache::get_page_id_for_size(const std::string &table_name, uint32_t data_size,
                                            page::PageType page_type) {
    switch (page_type) {
        case page::PageType::Data: {
            const auto &free_data_pages = _free_data_pages[table_name];
            auto data_page_id = std::find_if(
                free_data_pages.begin(),
                free_data_pages.end(),
                [&](const auto &kv) {
                    const auto&[page_id, free_space] = kv;
                    return free_space >= data_size;
                }
            );

            if (data_page_id != free_data_pages.end()) {
                return data_page_id->first;
            }
        }
        case page::PageType::RowMap: {
            const auto &free_rowmap_pages = _free_rowmap_pages[table_name];
            auto rowmap_page_id = std::find_if(
                free_rowmap_pages.begin(),
                free_rowmap_pages.end(),
                [&](const auto &kv) {
                    const auto&[page_id, free_space] = kv;
                    return free_space >= data_size;
                }
            );
            if (rowmap_page_id != free_rowmap_pages.end()) {
                return rowmap_page_id->first;
            }
        }
        case page::PageType::Metadata:
            return {};
    }

    return {};
}

std::string
page_cache::generate_cache_key(page::PageId page_id, const std::string &table_name, page::PageType page_type) {
    auto key = table_name + "::" + std::to_string(page_id);
    switch (page_type) {
        case page::PageType::Data:
            key = "data::" + key;
            break;
        case page::PageType::RowMap:
            key = "rm::" + key;
            break;
        case page::PageType::Metadata:
            key = "md::" + key;
            break;
    }

    return key;
}

std::string page_cache::file_name_from_table_name(const std::string &table_name) {
    return table_name + ".mbx";
}

