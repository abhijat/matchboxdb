#include <vector>
#include <iostream>
#include "test_utils.h"
#include "tuple.h"
#include "metadata.h"

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

int main() {
    pagetest::store_tuple_to_disk();
    pagetest::select_row();
}
