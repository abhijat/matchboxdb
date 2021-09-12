#ifndef MATCHBOXDB_METADATA_PAGE_H
#define MATCHBOXDB_METADATA_PAGE_H

#include "page.h"
#include "../storage/tuple.h"
#include "../storage/metadata.h"
#include "../storage/streamutils.h"

namespace page {

class MetadataPage : public Page {
public:
    MetadataPage(std::string table_name, std::vector<std::string> column_names,
                 std::vector<metadata::Kind> column_kinds, uint32_t n_data_pages,
                 uint32_t max_row_id, uint32_t total_pages);

    explicit MetadataPage(const stream_utils::ByteBuffer &buffer);

    MetadataPage(MetadataPage &&m) noexcept;

    stream_utils::ByteBuffer buffer() override;

    std::string to_string() const override;

    uint32_t free_space() const override;

    uint32_t n_data_pages() const;

    const std::string &table_name() const;

    const std::vector<std::string> &column_names() const;

    const std::vector<metadata::Kind> &column_kinds() const;

    uint32_t max_row_id() const;

    uint32_t next_row_id();

    uint32_t n_total_pages() const;

    uint32_t n_marked_pages() const;

    uint32_t n_unmarked_pages() const;

    stream_utils::ByteBuffer empty_page() override;

    bool has_pages_available() const;

    PageId next_page_id_for_table();

    void increment_marked_pages(PageType page_type);

protected:
    uint32_t _max_row_id{};
    uint32_t _n_data_pages{};
    uint32_t _n_total_pages{};
    uint32_t _n_marked_pages{};
    uint32_t _n_unmarked_pages{};
    std::string _table_name{};
    std::vector<std::string> _column_names{};
    std::vector<metadata::Kind> _column_kinds{};
};

}


#endif //MATCHBOXDB_METADATA_PAGE_H
