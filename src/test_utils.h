#ifndef MATCHBOXDB_TEST_UTILS_H
#define MATCHBOXDB_TEST_UTILS_H

#include <cstdint>
#include <vector>
#include <sstream>

#include "page/slotted_data_page.h"
#include "streamutils.h"
#include "page/metadata_page.h"
#include "table_initializer.h"
#include "page/page_cache.h"
#include "insert_object.h"
#include "select_object.h"

namespace pagetest {

std::vector<unsigned char> build_test_rowmap_page() {
    uint32_t header_size = 8 * sizeof(uint32_t);

    uint32_t page_id = 1;
    uint32_t next_page = 0;
    uint32_t prev_page = 0;
    uint32_t page_type = 0;
    uint32_t free_space = 4096 - header_size;

    uint32_t max_row_id = 0;
    uint32_t n_records = 0;

    std::stringstream s{std::ios::in | std::ios::out | std::ios::binary};
    s.write(reinterpret_cast<const char *>(&header_size), sizeof(header_size));
    s.write(reinterpret_cast<const char *>(&page_id), sizeof(page_id));
    s.write(reinterpret_cast<const char *>(&next_page), sizeof(next_page));
    s.write(reinterpret_cast<const char *>(&prev_page), sizeof(prev_page));
    s.write(reinterpret_cast<const char *>(&page_type), sizeof(page_type));
    s.write(reinterpret_cast<const char *>(&free_space), sizeof(free_space));
    s.write(reinterpret_cast<const char *>(&max_row_id), sizeof(max_row_id));
    s.write(reinterpret_cast<const char *>(&n_records), sizeof(n_records));

    uint32_t i = 0;
    auto page_size = 4096;
    const uint32_t record_size = 3 * sizeof(uint32_t);
    while (free_space >= record_size) {
        s.write(reinterpret_cast<const char *>(&i), sizeof(i));
        i += 1;
        s.write(reinterpret_cast<const char *>(&i), sizeof(i));
        i += 1;
        s.write(reinterpret_cast<const char *>(&i), sizeof(i));
        i += 1;
        free_space -= record_size;
        n_records += 1;
    }

    auto total_bytes = header_size + (n_records * record_size);
    auto padding_len = page_size - total_bytes;

    std::vector<unsigned char> padding(padding_len, '\0');
    s.write(reinterpret_cast<const char *>(padding.data()), padding_len);

    i -= 1;
    s.seekp(sizeof(uint32_t) * 5, std::ios::beg);
    s.write(reinterpret_cast<const char *>(&free_space), sizeof(free_space));

    s.write(reinterpret_cast<const char *>(&i), sizeof(i));
    s.write(reinterpret_cast<const char *>(&n_records), sizeof(n_records));

    auto data = s.str();
    return std::vector<unsigned char>{data.c_str(), data.c_str() + data.size()};
}

std::vector<unsigned char> build_test_data_page() {
    uint32_t header_size = 8 * sizeof(uint32_t);

    uint32_t page_id = 1;
    uint32_t next_page = 0;
    uint32_t prev_page = 0;
    uint32_t page_type = 1;
    uint32_t free_space = 4096 - header_size;

    uint32_t slot_end_marker = header_size;
    uint32_t tuple_begin_marker = 4096;

    std::stringstream s{std::ios::in | std::ios::out | std::ios::binary};
    s.write(reinterpret_cast<const char *>(&header_size), sizeof(header_size));
    s.write(reinterpret_cast<const char *>(&page_id), sizeof(page_id));
    s.write(reinterpret_cast<const char *>(&next_page), sizeof(next_page));
    s.write(reinterpret_cast<const char *>(&prev_page), sizeof(prev_page));
    s.write(reinterpret_cast<const char *>(&page_type), sizeof(page_type));
    s.write(reinterpret_cast<const char *>(&free_space), sizeof(free_space));
    s.write(reinterpret_cast<const char *>(&slot_end_marker), sizeof(slot_end_marker));
    s.write(reinterpret_cast<const char *>(&tuple_begin_marker), sizeof(tuple_begin_marker));

    auto padding_len = free_space;
    std::vector<unsigned char> padding(padding_len, '\0');

    s.write(reinterpret_cast<const char *>(padding.data()), padding_len);

    auto data = s.str();
    return std::vector<unsigned char>{data.c_str(), data.c_str() + data.size()};
}

stream_utils::ByteBuffer build_metadata_page() {
    page::MetadataPage m{
        "employee",
        {"firstname", "lastname", "age", "is_employed"},
        {metadata::Kind::String, metadata::Kind::String, metadata::Kind::UnsignedInt, metadata::Kind::Boolean},
        0,
        0,
        0,
        0
    };

    return m.buffer();
}

void test_metadata_page() {
    auto m = pagetest::build_metadata_page();
    auto r = page::MetadataPage{m};
    std::cout << r.to_string() << "\n";
}

void test_page_cache_scanner() {
    page_cache::PageCache pc{100, {"employee"}};
    for (auto i = 0; i < 200; ++i) {
        auto[r, d] = pc.get_pages_for_data_size("employee", 155);
    }
}

void store_tuple_to_disk() {
    std::string table{"employee"};
    page_cache::PageCache pc{100, {table}};

    tuple::Tuple t{{
                       std::string{"abhijat"},
                       std::string{"malviya"},
                       uint32_t{38},
                       true,
                   }};
    actions::InsertObject i{pc, table, t};
    auto row_id = i.save();
    std::cout << "saved data in row: " << row_id << "\n";

    pc.write_dirty_pages("employee");
}

void select_row() {
    std::string table{"employee"};
    page_cache::PageCache pc{100, {table}};

    actions::SelectObject s{pc, table, 1};
    auto tuple = s.get();

    std::cout << tuple << "\n";
}

}

#endif //MATCHBOXDB_TEST_UTILS_H
