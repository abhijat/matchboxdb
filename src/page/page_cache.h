#ifndef MATCHBOXDB_PAGE_CACHE_H
#define MATCHBOXDB_PAGE_CACHE_H

#include <cstdint>
#include <list>
#include <unordered_map>
#include <unordered_set>

#include "../storage/streamutils.h"
#include "metadata_page.h"
#include "page.h"
#include "slotted_data_page.h"

namespace page {
class PageDefragger;
}

namespace page_cache {

class PageCache {
public:
    static PageCache scan_and_build_page_cache(uint32_t max_size, const std::string &data_path_root);

    PageCache(uint32_t max_size, const std::vector<std::string> &tables);

    page::Page *get_page_id(page::PageId page_id, const std::string &table_name, page::PageType page_type);

    page::SlottedDataPage *get_page_for_data_size(const std::string &table_name, uint32_t data_size);

    uint32_t next_row_id_for_table(const std::string &table_name);

    page::MetadataPage *metadata_page_for_table(const std::string &table_name);

    metadata::Metadata metadata_for_table(const std::string &table_name);

    void write_dirty_pages(const std::string &table_name);

    std::vector<page::Page *> enumerate_pages(const std::string &table_name, page::PageType page_type);

    void mark_page_dirty(const std::string &table_name, page::Page *page, bool add_to_defrag_list);

    void add_new_table(const std::string &table_name);

    std::vector<std::string> table_names();

    void enable_page_defrag(page::PageDefragger *page_defragger);

protected:
    void scan_tables();

    void scan_table_file(const std::string &table_name, const std::string &file_name);

    virtual std::string evict();

    std::unique_ptr<page::Page>
    load_page_from_disk(page::PageId page_id, const std::string &table_name, page::PageType page_type);

protected:
    uint32_t _max_size{};
    std::unordered_map<std::string, std::unordered_set<page::Page *>> _dirty_pages{};
    std::unordered_map<std::string, std::string> _table_file_names{};
    std::unordered_map<std::string, std::vector<page::FreePageInfo>> _free_data_pages{};
    std::list<std::pair<std::string, std::unique_ptr<page::Page>>> _page_ids{};
    std::unordered_map<std::string, decltype(_page_ids.begin())> _pages{};

    std::unordered_map<std::string, std::unordered_map<page::PageType, std::vector<page::PageId>>> _page_directories{};

    void scan_free_pages_in_table_stream(const std::string &table_name, std::istream &is, uint32_t n_pages_to_scan);

    void handle_missing_cache_entry(unsigned int page_id, const std::string &table_name, page::PageType &page_type,
                                    const std::string &key);

    static std::unique_ptr<page::Page>
    make_page_from_buffer(const page::PageType &page_type, const stream_utils::ByteBuffer &buffer);

    std::optional<page::PageId>
    get_page_id_for_size(const std::string &table_name, uint32_t data_size, page::PageType page_type);

    std::optional<page::PageDefragger*> _page_defragger{};
};

std::string generate_cache_key(page::PageId page_id, const std::string &table_name, page::PageType page_type);

}


#endif //MATCHBOXDB_PAGE_CACHE_H
