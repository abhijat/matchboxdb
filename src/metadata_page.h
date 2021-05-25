#ifndef MATCHBOXDB_METADATA_PAGE_H
#define MATCHBOXDB_METADATA_PAGE_H

#include "page.h"
#include "tuple.h"
#include "metadata.h"
#include "streamutils.h"

namespace page {

class MetadataPage : public Page {
public:
    MetadataPage(std::string table_name, std::vector<std::string> column_names,
                 std::vector<metadata::Kind> column_kinds, uint32_t n_data_pages,
                 uint32_t n_rowmap_pages, uint32_t max_row_id);

    explicit MetadataPage(const stream_utils::ByteBuffer &buffer);

    MetadataPage(MetadataPage &&m) noexcept;

    stream_utils::ByteBuffer buffer() override;

    std::string to_string() const override;

    uint32_t free_space() const override;

    uint32_t n_data_pages() const;

    uint32_t n_rowmap_pages() const;

    const std::string &table_name() const;

    const std::vector<std::string> &column_names() const;

    const std::vector<metadata::Kind> &column_kinds() const;

    uint32_t max_row_id() const;

    uint32_t next_row_id();

protected:
    uint32_t _max_row_id{};
    uint32_t _n_data_pages{};
    uint32_t _n_rowmap_pages{};
    std::string _table_name{};
    std::vector<std::string> _column_names{};
    std::vector<metadata::Kind> _column_kinds{};
};

}


#endif //MATCHBOXDB_METADATA_PAGE_H
