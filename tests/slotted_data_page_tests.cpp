#include <gtest/gtest.h>

#include "../src/slotted_data_page.h"

tuple::Tuple make_test_tuple() {
    return tuple::Tuple{{std::string{"abhijat"}, uint32_t{38}, true}};
}

tuple::Tuple make_tuple_from_buffer(const stream_utils::ByteBuffer &buf) {
    metadata::Metadata m{{"name",                 "age",                       "is_employed"},
                         {metadata::Kind::String, metadata::Kind::UnsignedInt, metadata::Kind::Boolean}};
    return tuple::Tuple{buf, m};
}

page::SlottedDataPage make_empty_page(uint32_t page_size = page::k_page_size) {
    page::SlottedDataPage slotted_data_page{0, page_size};
    auto buffer = slotted_data_page.empty_page();
    return page::SlottedDataPage{buffer};
}

TEST(SlottedDataPageTests, EmptyPage) {
    page::SlottedDataPage slotted_data_page{0, page::k_page_size};
    auto buffer = slotted_data_page.empty_page();

    page::SlottedDataPage new_page{buffer};
    ASSERT_EQ(new_page.page_id(), slotted_data_page.page_id());
    ASSERT_EQ(new_page.header_size(), slotted_data_page.header_size());
    ASSERT_EQ(new_page.slot_end_marker(), new_page.header_size());
    ASSERT_EQ(new_page.tuple_begin_marker(), page::k_page_size);
}

TEST(SlottedDataPageTests, StoreTuple) {
    auto p = make_empty_page();
    auto tuple = make_test_tuple();

    auto free_space_before = p.free_space();
    auto id = p.store_tuple(tuple);
    ASSERT_EQ(tuple.size() + 2 * sizeof(uint32_t), free_space_before - p.free_space());
    ASSERT_EQ(id, 0);
}

TEST(SlottedDataPageTests, ReadStoredTuple) {
    auto p = make_empty_page();
    auto tuple = make_test_tuple();
    auto buf = p.read_tuple_from_slot(p.store_tuple(tuple));
    ASSERT_EQ(make_tuple_from_buffer(buf), tuple);
}

TEST(SlottedDataPageTests, FailWhenNoSpace) {
    auto p = make_empty_page(page::SlottedDataPage::header_size());
    auto tuple = make_test_tuple();
    EXPECT_THROW(p.store_tuple(tuple), std::out_of_range);
}