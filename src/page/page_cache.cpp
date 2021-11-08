#include "page_cache.h"

#include <fstream>
#include <iostream>
#include <numeric>
#include <utility>

#include "../logging.h"
#include "../storage/utils.h"
#include "defragger.h"
#include "page.h"
#include "page_creator.h"
#include "page_scanner.h"
#include "slotted_data_page.h"

page_cache::PageCache::PageCache(uint32_t max_size, const std::vector<std::string> &tables)
    : _max_size{max_size} {
    for (const auto &table: tables) {
        _table_file_names.insert({table, storage_utils::file_name_from_table_name(table)});
    }

    scan_tables();
}

page::Page *
page_cache::PageCache::get_page_id(page::PageId page_id, const std::string &table_name, page::PageType page_type) {
    log::info("looking for page id [", page_id, "], table name [", table_name, "], page type", page_type);
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
        evict();
    }

    _page_ids.push_front(std::make_pair(key, std::move(page)));
    _pages.insert({key, _page_ids.begin()});
}

std::unique_ptr<page::Page>
page_cache::PageCache::load_page_from_disk(page::PageId page_id, const std::string &table_name,
                                           page::PageType page_type) {
    auto table_file_name = _table_file_names[table_name];
    std::ifstream i{table_file_name, std::ios::binary};

    if (!i) {
        log::info("failed to open table file:", table_file_name, "for table", table_name);
        // TODO - table does not exist error
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
        case page::PageType::Metadata:
            return std::make_unique<page::MetadataPage>(buffer);
        default:
            throw std::invalid_argument{"Invalid page type!"};
    }
}

std::string page_cache::PageCache::evict() {
    // TODO lock here
    auto page_id_to_evict = _page_ids.back().first;
    _page_ids.pop_back();
    _pages.erase(page_id_to_evict);
    return page_id_to_evict;
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

page::SlottedDataPage *
page_cache::PageCache::get_page_for_data_size(const std::string &table_name, uint32_t data_size) {
    log::info("finding pages for table", table_name, "for data size", data_size);
    auto data_page_id_maybe = get_page_id_for_size(table_name, data_size, page::PageType::Data);
    auto metadata_page = metadata_page_for_table(table_name);

    page::PageCreator page_creator{table_name, metadata_page};

    uint32_t data_page_id;
    bool page_created = false;

    if (data_page_id_maybe) {
        data_page_id = *data_page_id_maybe;
    } else {
        data_page_id = page_creator.create_page(page::PageType::Data);
        _page_directories[table_name][page::PageType::Data].push_back(data_page_id);
        page_created = true;
    }

    auto *data_page = dynamic_cast<page::SlottedDataPage *>(get_page_id(data_page_id, table_name,
                                                                        page::PageType::Data));

    mark_page_dirty(table_name, data_page, false);
    mark_page_dirty(table_name, metadata_page, false);

    if (page_created) {
        _free_data_pages[table_name].emplace_back(data_page_id, data_page->free_space());
    }
    return data_page;
}

uint32_t page_cache::PageCache::next_row_id_for_table(const std::string &table_name) {
    return metadata_page_for_table(table_name)->next_row_id();
}

void page_cache::PageCache::write_dirty_pages(const std::string &table_name) {
    if (_dirty_pages.empty() || !_dirty_pages.contains(table_name)) {
        log::info("skipping dirty page write, no entries");
        return;
    }

    std::fstream ofs{storage_utils::file_name_from_table_name(table_name),
                     std::ios::binary | std::ios::in | std::ios::out};
    if (!ofs) {
        throw std::ios::failure{"failed to read table file"};
    }

    for (const auto &page_ptr: _dirty_pages.at(table_name)) {
        log::info("writing dirty page", page_ptr->page_type(), "to disk for table", table_name);
        ofs.seekp(page_ptr->page_id() * page::k_page_size, std::ios::beg);
        ofs.write(reinterpret_cast<const char *>(page_ptr->buffer().data()), page::k_page_size);
        if (!ofs) {
            throw std::ios::failure{"failed to write page to file"};
        }
    }

    _dirty_pages[table_name].clear();
}

page::MetadataPage *page_cache::PageCache::metadata_page_for_table(const std::string &table_name) {
    return dynamic_cast<page::MetadataPage *>(get_page_id(0, table_name, page::PageType::Metadata));
}

void page_cache::PageCache::scan_free_pages_in_table_stream(const std::string &table_name, std::istream &is,
                                                            uint32_t n_pages_to_scan) {
    page_visitors::FreePageCollector free_page_collector{};
    page_scan_utils::PageScanner page_scanner{is, free_page_collector};

    auto page_directory = page_scanner.scan_pages();

    _free_data_pages.emplace(table_name, free_page_collector.free_data_pages());
    _page_directories.emplace(table_name, page_directory);
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
        case page::PageType::Metadata:
            return {};
    }

    return {};
}

std::vector<page::Page *>
page_cache::PageCache::enumerate_pages(const std::string &table_name, page::PageType page_type) {
    auto page_ids = _page_directories[table_name][page_type];
    std::vector<page::Page *> pages{};

    std::transform(page_ids.cbegin(), page_ids.cend(), std::back_inserter(pages), [&](auto page_id) {
        return get_page_id(page_id, table_name, page_type);
    });

    return pages;
}

void page_cache::PageCache::mark_page_dirty(const std::string &table_name, page::Page *page, bool add_to_defrag_list) {
    auto pos = _dirty_pages.find(table_name);
    if (pos == _dirty_pages.end()) {
        _dirty_pages[table_name] = {page};
    } else {
        _dirty_pages[table_name].emplace(page);
    }

    // this will hang if the page mutex is already claimed.
    if (add_to_defrag_list) {
        page->update_version();
    }

    if (_page_defragger && add_to_defrag_list) {
        const auto *slotted_data_page = dynamic_cast<const page::SlottedDataPage *>(page);

        // defrag is not supported for non-data pages yet
        if (slotted_data_page) {
            (*_page_defragger)->add_page_to_defrag_list(slotted_data_page, table_name);
        }
    }
}

metadata::Metadata page_cache::PageCache::metadata_for_table(const std::string &table_name) {
    auto page = metadata_page_for_table(table_name);
    return metadata::Metadata{page->column_names(), page->column_kinds()};
}

void page_cache::PageCache::add_new_table(const std::string &table_name) {
    auto file_name = storage_utils::file_name_from_table_name(table_name);
    _table_file_names.insert({table_name, file_name});
    scan_table_file(table_name, file_name);
}

page_cache::PageCache
page_cache::PageCache::scan_and_build_page_cache(uint32_t max_size, const std::string &data_path_root) {
    log::info("scanning for tables in path", data_path_root);
    auto tables = storage_utils::scan_table_names_from_files(data_path_root);
    auto found_tables = std::accumulate(
        tables.cbegin(),
        tables.cend(),
        std::string{},
        [](auto a, auto b) {
            return a.empty() ? b : a + ", " + b;
        }
    );
    log::info("found tables: [", found_tables, "]");
    return PageCache{max_size, tables};
}

std::vector<std::string> page_cache::PageCache::table_names() {
    std::vector<std::string> table_names{_table_file_names.size()};
    std::transform(_table_file_names.cbegin(), _table_file_names.cend(), table_names.begin(), [](const auto &a) {
        return a.first;
    });
    return table_names;
}

void page_cache::PageCache::enable_page_defrag(page::PageDefragger *page_defragger) {
    _page_defragger = page_defragger;
}

std::string
page_cache::generate_cache_key(page::PageId page_id, const std::string &table_name, page::PageType page_type) {
    auto key = table_name + "::" + std::to_string(page_id);
    switch (page_type) {
        case page::PageType::Data:
            key = "data::" + key;
            break;
        case page::PageType::Metadata:
            key = "md::" + key;
            break;
    }

    return key;
}
