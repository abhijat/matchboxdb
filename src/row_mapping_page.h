#ifndef MATCHBOXDB_ROW_MAPPING_PAGE_H
#define MATCHBOXDB_ROW_MAPPING_PAGE_H

#include "page.h"

namespace page {

const uint32_t k_record_width{3 * sizeof(uint32_t)};

class RowMappingPage : public Page {
public:
    explicit RowMappingPage(const std::vector<unsigned char> &buffer);

    RowMappingPage(uint32_t page_id, uint32_t page_size);

    std::vector<Record> enumerate_records();

    std::string to_string() const override;

    uint32_t free_space() const override;

    stream_utils::ByteBuffer buffer() override;

    stream_utils::ByteBuffer empty_page();

    void store_record(const Record &record);

    Record record_for_row_id(uint32_t row_id);


protected:
    static constexpr uint32_t header_size();
    uint32_t record_size() const;

    uint32_t _max_row_id{};
    uint32_t _n_records{};
};

}


#endif //MATCHBOXDB_ROW_MAPPING_PAGE_H
