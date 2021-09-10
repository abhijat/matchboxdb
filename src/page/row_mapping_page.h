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

    stream_utils::ByteBuffer empty_page() override;

    void store_record(const Record &record);

    Record record_for_row_id(uint32_t row_id);

    static constexpr uint32_t header_size() {
        return k_base_header_size + sizeof(_n_records) + sizeof(_max_row_id);
    }

protected:
    uint32_t record_size() const;

    uint32_t _max_row_id{};
    uint32_t _n_records{};
};

}


#endif //MATCHBOXDB_ROW_MAPPING_PAGE_H
