#ifndef MATCHBOXDB_PAGE_H
#define MATCHBOXDB_PAGE_H


#include <cstdint>
#include <memory>
#include <vector>
#include <sstream>
#include <list>
#include <unordered_map>
#include <mutex>

#include "../storage/streamutils.h"

namespace page {


using PageId = uint32_t;

using FreePageInfo = std::pair<PageId, uint32_t>;

const uint32_t k_page_size{4096};
const uint32_t k_base_header_size{sizeof(uint32_t) * 6};

enum class PageType {
    Data,
    Metadata,
};

std::ostream &operator<<(std::ostream &os, PageType page_type);

struct Record {
    Record(uint32_t row_id, uint32_t page_id, uint32_t slot_id);

    uint32_t row_id;
    uint32_t page_id;
    uint32_t slot_id;

    static Record read_from_stream(std::stringstream &s);

    std::stringstream &write_to_stream(std::stringstream &s) const;

    [[nodiscard]] std::string to_string() const;
};

struct TupleWithSlotId {
    uint32_t slot_id;
    stream_utils::ByteBuffer byte_buffer;
};

class Page {
public:
    explicit Page(std::vector<unsigned char> buffer);

    Page(uint32_t page_id, uint32_t next_page_id, uint32_t prev_page_id, page::PageType page_type,
         uint32_t page_size, uint32_t free_space);

    Page();

    Page(Page &&p) noexcept;

    virtual std::string to_string() const = 0;

    virtual uint32_t free_space() const = 0;

    virtual stream_utils::ByteBuffer buffer() = 0;

    uint32_t page_id() const;

    uint32_t next_page_id() const;

    uint32_t prev_page_id() const;

    PageType page_type() const;

    uint32_t page_size() const;

    uint32_t version() const;

    void update_version();

    /**
     * Returns a buffer from the page, with the understanding that the buffer does not contain anything other than
     * the header fields. The remaining page is filled with null bytes. Useful when initializing the table file.
     */
    virtual stream_utils::ByteBuffer empty_page() = 0;

    std::mutex &mutex();

protected:
    void write_header_to_stream();

    void read_header();

    PageType read_page_type();

    void write_page_type();

protected:
    stream_utils::ByteBuffer _buffer;
    std::stringstream _stream;

    uint32_t _base_header_size{};
    uint32_t _page_id{};
    uint32_t _next_page_id{};
    uint32_t _prev_page_id{};
    PageType _page_type;
    uint32_t _page_size{};
    uint32_t _free_space{};

    std::mutex _mutex{};

    uint32_t _version{};
};

PageType page_type_from_buffer(const stream_utils::ByteBuffer &buffer);

uint32_t free_space_from_buffer(const stream_utils::ByteBuffer &buffer);

PageId page_id_from_buffer(const stream_utils::ByteBuffer &buffer);

}

#endif //MATCHBOXDB_PAGE_H
