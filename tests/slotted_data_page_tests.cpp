#include <gtest/gtest.h>

#include "../src/page/slotted_data_page.h"

tuple::Tuple make_test_tuple() {
    return tuple::Tuple{{std::string{"abhijat"}, uint32_t{38}, true}};
}

metadata::Metadata make_metadata() {
    return {{"name",                 "age",                       "is_employed"},
            {metadata::Kind::String, metadata::Kind::UnsignedInt, metadata::Kind::Boolean}};
}

tuple::Tuple make_tuple_from_buffer(const stream_utils::ByteBuffer &buf) {
    return tuple::Tuple{buf, make_metadata()};
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

TEST(SlottedDataPageTests, DeleteTuple) {
    auto p = make_empty_page();
    auto tuple = make_test_tuple();
    p.store_tuple(tuple);
    ASSERT_EQ(p.enumerate_tuples().size(), 1);

    p.delete_tuple_at_slot_id(0);
    ASSERT_EQ(p.enumerate_tuples().size(), 0);
}

TEST(SlottedDataPageTests, DeleteTupleDoesNotChangeSize) {
    auto p = make_empty_page();
    auto tuple = make_test_tuple();
    p.store_tuple(tuple);
    auto free_space_before = p.free_space();

    p.delete_tuple_at_slot_id(0);
    ASSERT_EQ(p.free_space(), free_space_before);
}

TEST(SlottedDataPageTests, FailWhenDeletedSlotIdOverrunsSlotMarker) {
    auto p = make_empty_page();
    EXPECT_THROW(p.delete_tuple_at_slot_id(100), std::out_of_range);
}

TEST(SlottedDataPageTests, DefragPage) {
    auto p = make_empty_page();
    p.store_tuple(tuple::Tuple{{std::string{"alpha"}, uint32_t{38}, true}});
    p.store_tuple(tuple::Tuple{{std::string{"bravo"}, uint32_t{38}, true}});
    p.store_tuple(tuple::Tuple{{std::string{"charlie"}, uint32_t{38}, true}});
    p.store_tuple(tuple::Tuple{{std::string{"delta"}, uint32_t{38}, true}});
    ASSERT_EQ(p.enumerate_tuples().size(), 4);

    p.delete_tuple_at_slot_id(0);
    p.delete_tuple_at_slot_id(2);

    auto before = p.free_space();
    auto saved = p.defrag_page(make_metadata());
    auto after = p.free_space();
    ASSERT_EQ(saved, after - before);
}
