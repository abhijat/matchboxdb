#ifndef MATCHBOXDB_PAGE_CACHE_H
#define MATCHBOXDB_PAGE_CACHE_H

#include <list>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>

#include "page.h"
#include "streamutils.h"
#include "slotted_data_page.h"
#include "metadata_page.h"
#include "row_mapping_page.h"

namespace page_cache {

using PageId = uint32_t;

using FreePageInfo = std::pair<PageId, uint32_t>;

class PageCache {
public:
    explicit PageCache(uint32_t max_size, const std::vector<std::string> &tables);

    page::Page *get_page_id(PageId page_id, const std::string &table_name, page::PageType page_type);

    std::pair<page::RowMappingPage *, page::SlottedDataPage *>
    get_pages_for_data_size(const std::string &table_name, uint32_t data_size);

    uint32_t row_id_for_table(const std::string &table_name);

    page::RowMappingPage *rowmap_page_for_row_id(const std::string &table_name, uint32_t row_id);

    page::MetadataPage *metadata_page_for_table(const std::string& table_name);

    void write_dirty_pages(const std::string &table_name);

protected:
    void scan_tables();

    void scan_table_file(const std::string &table_name, const std::string &file_name);

    void evict();

    std::unique_ptr<page::Page>
    load_page_from_disk(PageId page_id, const std::string &table_name, page::PageType page_type);

protected:
    uint32_t _max_size{};
    std::unordered_map<std::string, std::unordered_set<page::Page *>> _dirty_pages{};
    std::unordered_map<std::string, std::string> _table_file_names{};
    std::unordered_map<std::string, std::vector<FreePageInfo>> _free_data_pages{};
    std::unordered_map<std::string, std::vector<FreePageInfo>> _free_rowmap_pages{};
    std::list<std::pair<std::string, std::unique_ptr<page::Page>>> _page_ids{};
    std::unordered_map<std::string, decltype(_page_ids.begin())> _pages{};

    static std::vector<page_cache::FreePageInfo>
    scan_data_pages_from_tablestream(std::ifstream &ifs, const page::MetadataPage &m);

    static std::vector<page_cache::FreePageInfo>
    scan_rowmap_pages_from_tablestream(std::ifstream &ifs, const page::MetadataPage &m);

    void handle_missing_cache_entry(unsigned int page_id, const std::string &table_name, page::PageType &page_type,
                                    const std::string &key);

    static std::unique_ptr<page::Page>
    make_page_from_buffer(const page::PageType &page_type, const stream_utils::ByteBuffer &buffer);
};

std::ifstream &seek_to_data_page_offset(PageId page_id, std::ifstream &ifs);

std::ifstream &seek_to_rowmap_page_offset(PageId page_id, std::ifstream &ifs);

std::string generate_cache_key(page_cache::PageId page_id, const std::string &table_name, page::PageType page_type);

std::string file_name_from_table_name(const std::string &table_name);

}


#endif //MATCHBOXDB_PAGE_CACHE_H
