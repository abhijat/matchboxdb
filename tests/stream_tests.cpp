#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "../src/streamutils.h"

TEST(Strings, StringSerializeAndDeserialize) {
    auto s = stream_utils::build_binary_stream();
    stream_utils::write_string_to_stream(s, "abhijat");
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

TEST(Streams, BufferAndStream) {
    auto s = stream_utils::build_binary_stream();
    stream_utils::write_data_to_stream(s, uint32_t{100});
    stream_utils::write_data_to_stream(s, uint32_t{100});
    stream_utils::write_data_to_stream(s, uint32_t{100});

    auto buffer = stream_utils::buffer_from_stream(s);

    auto a = stream_utils::build_binary_stream();
    stream_utils::load_stream_from_buffer(s, buffer);

    for (auto i = 0; i < 3; ++i) {
        auto item = stream_utils::read_data_from_stream<uint32_t>(s);
        ASSERT_EQ(item, 100);
    }
}

TEST(Streams, WriteBufferToStream) {
    auto s = stream_utils::build_binary_stream();
    stream_utils::ByteBuffer buffer(100, '\0');
    stream_utils::write_byte_buffer_to_stream(s, buffer);
    ASSERT_EQ(s.str().size(), 100 + sizeof(uint32_t));
}

TEST(Sizes, SizeOfString) {
    std::string s{"angry birds"};
    auto size = stream_utils::size_of_string(s);
    ASSERT_EQ(size, s.size() + sizeof(uint32_t));
}

TEST(Sizes, SizeOfStringVector) {
    std::vector<std::string> v{"foo", "bar", "baz"};
    auto size = stream_utils::size_of_strings(v);
    ASSERT_EQ(size, sizeof(uint32_t) + (v.size() * (3 + sizeof(uint32_t))));
}

TEST(Sizes, SizeOfKind) {
    metadata::Kind k{metadata::Kind::Boolean};
    auto size = stream_utils::size_of_kind(k);
    ASSERT_EQ(size, sizeof(unsigned char));
}

TEST(Sizes, SizeOfKinds) {
    std::vector<metadata::Kind> kinds{metadata::Kind::Boolean, metadata::Kind::String};
    auto size = stream_utils::size_of_kinds(kinds);
    ASSERT_EQ(size, sizeof(uint32_t) + (kinds.size() * sizeof(unsigned char)));
}
