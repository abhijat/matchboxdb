#include <vector>
#include <iostream>
#include "file_read.h"
#include "file_write.h"
#include "test_utils.h"
#include "tuple.h"
#include "metadata.h"

void build_db() {
    storage::create_empty_file_of_size("mbx.db", 1);
    storage::initialize_database("mbx.db");
}

void read_db() {
    std::ifstream ifs;
    ifs.open("mbx.db", std::ios::binary);
    storage::read_page_directory(ifs);
}

uint32_t write_tuple_to_page(page::SlottedDataPage &page, tuple::Tuple &&tuple) {
    auto slot_id = page.store_tuple(tuple);
    return slot_id;
}

void test_data_page() {
    auto data = pagetest::build_test_data_page();
    std::cout << "built test page of size: " << data.size() << "\n";

    page::SlottedDataPage page{data};


    std::vector<uint32_t> slot_ids;

    slot_ids.push_back(write_tuple_to_page(page, std::move(tuple::Tuple({
                                                                            std::string{"abhijat"},
                                                                            false,
                                                                            uint32_t{38}
                                                                        }))));

    slot_ids.push_back(write_tuple_to_page(page, std::move(tuple::Tuple({
                                                                            std::string{"anushree malviya"},
                                                                            true,
                                                                            uint32_t{34}
                                                                        }))));

    slot_ids.push_back(write_tuple_to_page(page, std::move(tuple::Tuple({
                                                                            std::string{
                                                                                "the quick brown fox jumped over the lazy doge"},
                                                                            true,
                                                                            uint32_t{1000}
                                                                        }))));

    metadata::Metadata m{
        {"name",                 "is_short",              "age"},
        {metadata::Kind::String, metadata::Kind::Boolean, metadata::Kind::UnsignedInt},
    };

    for (const auto &slot_id : slot_ids) {
        auto buffer = page.read_tuple_from_slot(slot_id);
        tuple::Tuple t{buffer, m};
        std::cout << t << "\n";
    }

}

void make_test_tuple() {
    metadata::Metadata m{
        {"name",                 "is_short",              "age"},
        {metadata::Kind::String, metadata::Kind::Boolean, metadata::Kind::UnsignedInt},
    };

    tuple::Tuple t{{std::string{"abhijat"}, false, uint32_t{38}}};
    auto buf = t.data();

    tuple::Tuple foo{buf, m};
    std::cout << foo << "\n";
}

int main() {
//    pagetest::test_table_init();
//    pagetest::store_tuple_to_disk();
    pagetest::select_row();
}
