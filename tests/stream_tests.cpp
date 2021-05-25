#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../src/streamutils.h"

TEST(Strings, StringSerializeAndDeserialize) {
    auto s = stream_utils::build_binary_stream();
    stream_utils::write_string_to_stream(s, "abhijat");
    s.seekg(0, std::ios::beg);
    auto read_back = stream_utils::read_string_from_stream(s);
    ASSERT_EQ(read_back, "abhijat");
}

TEST(Strings, StringVectorTest) {
    auto s = stream_utils::build_binary_stream();
    std::vector<std::string> data{"foo", "bar", "baz"};
    stream_utils::write_strings_to_stream(s, data);
    auto read_back = stream_utils::read_strings_from_stream(s);
    ASSERT_THAT(read_back, ::testing::ContainerEq(decltype(data){"foo", "bar", "baz"}));
}