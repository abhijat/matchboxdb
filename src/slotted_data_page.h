#ifndef MATCHBOXDB_SLOTTED_DATA_PAGE_H
#define MATCHBOXDB_SLOTTED_DATA_PAGE_H

#include "page.h"
#include "tuple.h"

namespace page {

class SlottedDataPage : public Page {
public:
    explicit SlottedDataPage(const std::vector<unsigned char> &buffer);

    SlottedDataPage(uint32_t page_id, uint32_t page_size);

    uint32_t store_tuple(const tuple::Tuple &tuple);

    std::vector<unsigned char> read_tuple_from_slot(uint32_t slot_id);

    std::string to_string() const override;

    stream_utils::ByteBuffer buffer() override;

    uint32_t free_space() const override;

    stream_utils::ByteBuffer empty_page() override;

    uint32_t slot_end_marker() const;

    uint32_t tuple_begin_marker() const;

    static constexpr uint32_t header_size() {
        return k_base_header_size + sizeof(_slot_end_marker) + sizeof(_tuple_begin_marker);
    }

protected:

    uint32_t _slot_end_marker{};
    uint32_t _tuple_begin_marker{};
};

}


#endif //MATCHBOXDB_SLOTTED_DATA_PAGE_H
