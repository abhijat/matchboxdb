#ifndef MATCHBOXDB_SLOTTED_DATA_PAGE_H
#define MATCHBOXDB_SLOTTED_DATA_PAGE_H

#include "page.h"
#include "../storage/tuple.h"

namespace page {

class SlottedDataPage : public Page {
public:
    explicit SlottedDataPage(const std::vector<unsigned char> &buffer);

    SlottedDataPage(uint32_t page_id, uint32_t page_size);

    SlottedDataPage(const SlottedDataPage &slotted_data_page);

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

    std::vector<TupleWithSlotId> enumerate_tuples();

    /**
     * Soft deletes tuple at slot id, by replacing the offset at slot with tombstone marker 0
     */
    void delete_tuple_at_slot_id(uint32_t slot_id);

    /**
     *  Removes deleted tuples from the page, by re-writing un-deleted tuples over deleted tuples.
     *  Slot ids are changed and free space should increase after running.
     */
    uint32_t defrag_page(const metadata::Metadata &metadata);

    void overwrite_with(const SlottedDataPage& slotted_data_page);

protected:
    /**
     * Resets the tuple start and slot end markers, in preparation for a defrag.
     * Also resets free space.
     */
    void reset_markers();

    std::vector<stream_utils::ByteBuffer> load_tuple_buffers(const std::vector<uint32_t>& offsets);

    stream_utils::ByteBuffer load_tuple_buffer(uint32_t offset);

protected:

    uint32_t _slot_end_marker{};
    uint32_t _tuple_begin_marker{};
};

}


#endif //MATCHBOXDB_SLOTTED_DATA_PAGE_H
