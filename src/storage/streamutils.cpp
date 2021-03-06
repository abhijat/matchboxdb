#include <iostream>
#include <numeric>
#include <fstream>

#include "streamutils.h"
#include "../page/metadata_page.h"
#include "../page/slotted_data_page.h"

std::stringstream stream_utils::build_binary_stream() {
    return std::stringstream{std::ios::binary | std::ios::in | std::ios::out};
}

std::string stream_utils::read_string_from_stream(std::stringstream &s) {
    auto size = read_data_from_stream<uint32_t>(s);

    char buf[size];
    s.read(buf, size);
    if (!s) {
        throw std::ios::failure("failed to read_string_from_stream");
    }
    return std::string{buf, size};
}

std::stringstream &stream_utils::write_string_to_stream(std::stringstream &s, const std::string &data) {
    uint32_t size = data.size();
    write_data_to_stream(s, size);

    s.write(data.c_str(), size);

    if (!s) {
        throw std::ios::failure("failed to write_string_to_stream");
    }

    return s;
}

std::stringstream &
stream_utils::write_byte_buffer_to_stream(std::stringstream &s, const std::vector<unsigned char> &buffer) {
    uint32_t size = buffer.size();
    write_data_to_stream(s, size);

    s.write(reinterpret_cast<const char *>(buffer.data()), size);
    if (!s) {
        throw std::ios::failure("failed to write_byte_buffer_to_stream");
    }

    return s;
}

std::stringstream &stream_utils::write_kind_to_stream(std::stringstream &s, metadata::Kind kind) {
    auto as_int = static_cast<unsigned char>(kind);
    write_data_to_stream(s, as_int);
    return s;
}

metadata::Kind stream_utils::read_kind_from_stream(std::stringstream &s) {
    auto as_int = read_data_from_stream<unsigned char>(s);
    return static_cast<metadata::Kind>(as_int);
}

std::stringstream &stream_utils::write_strings_to_stream(std::stringstream &s, const std::vector<std::string> &items) {
    write_data_to_stream<uint32_t>(s, items.size());
    for (const auto &item: items) {
        write_string_to_stream(s, item);
    }
    return s;
}

std::vector<std::string> stream_utils::read_strings_from_stream(std::stringstream &s) {
    auto size = read_data_from_stream<uint32_t>(s);
    std::vector<std::string> items(size);

    for (auto i = 0; i < size; ++i) {
        items[i] = read_string_from_stream(s);
    }

    return items;
}

std::stringstream &stream_utils::write_kinds_to_stream(std::stringstream &s, const std::vector<metadata::Kind> &kinds) {
    write_data_to_stream(s, uint32_t(kinds.size()));
    for (const auto &kind: kinds) {
        write_kind_to_stream(s, kind);
    }

    return s;
}

std::vector<metadata::Kind> stream_utils::read_kinds_from_stream(std::stringstream &s) {
    auto size = read_data_from_stream<uint32_t>(s);
    std::vector<metadata::Kind> kinds(size);

    for (auto i = 0; i < size; ++i) {
        kinds[i] = read_kind_from_stream(s);
    }

    return kinds;
}

std::stringstream &
stream_utils::load_stream_from_buffer(std::stringstream &s, const stream_utils::ByteBuffer &byte_buffer) {
    s.write(reinterpret_cast<const char *>(byte_buffer.data()), static_cast<uint32_t>(byte_buffer.size()));
    if (!s) {
        throw std::ios::failure("failed to load_stream_from_buffer");
    }
    return s;
}

stream_utils::ByteBuffer stream_utils::buffer_from_stream(std::stringstream &s) {
    auto str = s.str();
    return ByteBuffer{str.data(), str.data() + str.size()};
}

uint32_t stream_utils::size_of_string(const std::string &s) {
    return sizeof(uint32_t) + s.size();
}

uint32_t stream_utils::size_of_strings(const std::vector<std::string> &strings) {
    auto component_sizes = std::transform_reduce(
        strings.cbegin(),
        strings.cend(),
        0,
        [](auto a, auto b) { return a + b; },
        [](const auto &s) { return size_of_string(s); }
    );
    return sizeof(uint32_t) + component_sizes;
}

uint32_t stream_utils::size_of_kind() {
    return sizeof(unsigned char);
}

uint32_t stream_utils::size_of_kinds(const std::vector<metadata::Kind> &kinds) {
    return sizeof(uint32_t) + sizeof(unsigned char) * kinds.size();
}

page::MetadataPage stream_utils::read_nth_metadata_page(std::istream &is, uint32_t n) {
    is.seekg(0, std::ios::beg);
    return page::MetadataPage(read_page_from_stream(is));
}

std::fstream stream_utils::open_db_file(const std::string &file_name) {
    std::fstream f{file_name, std::ios::binary | std::ios::out | std::ios::in};
    if (!f) {
        throw std::ios::failure{"failed to open_db_file"};
    }

    return f;
}

std::fstream &stream_utils::write_page_to_stream(std::fstream &f, const stream_utils::ByteBuffer &buffer) {
    f.write(reinterpret_cast<const char *>(buffer.data()), page::k_page_size);
    return f;
}

stream_utils::ByteBuffer stream_utils::read_page_from_stream(std::istream &is) {
    ByteBuffer buffer(page::k_page_size);
    is.read(reinterpret_cast<char *>(buffer.data()), page::k_page_size);
    return buffer;
}

stream_utils::ByteBuffer stream_utils::read_page_from_stream(std::istream &is, page::PageId page_id) {
    is.seekg(page::k_page_size * page_id, std::ios::beg);
    return read_page_from_stream(is);
}

