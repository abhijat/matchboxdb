#ifndef MATCHBOXDB_STREAMUTILS_H
#define MATCHBOXDB_STREAMUTILS_H

#include <cstdint>
#include <string>
#include <sstream>
#include <vector>

#include "metadata.h"

namespace stream_utils {

using ByteBuffer = std::vector<unsigned char>;

std::stringstream build_binary_stream();

std::stringstream &load_stream_from_buffer(std::stringstream &s, const ByteBuffer &byte_buffer);

ByteBuffer buffer_from_stream(std::stringstream &s);

template<typename T>
T read_data_from_stream(std::stringstream &s) {
    T t;
    s.read(reinterpret_cast<char *>(&t), sizeof(t));
    if (!s) {
        throw std::ios::failure("failed to read_data_from_stream");
    }
    return t;
}

std::string read_string_from_stream(std::stringstream &s);

template<typename T>
std::stringstream &write_data_to_stream(std::stringstream &s, T data) {
    s.write(reinterpret_cast<char *>(&data), sizeof(data));
    if (!s) {
        throw std::ios::failure("failed to write_data_to_stream");
    }
    return s;
}

std::stringstream &write_string_to_stream(std::stringstream &s, const std::string &data);

std::stringstream &write_byte_buffer_to_stream(std::stringstream &s, const std::vector<unsigned char> &buffer);

std::stringstream &write_kind_to_stream(std::stringstream &s, metadata::Kind kind);

metadata::Kind read_kind_from_stream(std::stringstream &s);

std::stringstream &write_strings_to_stream(std::stringstream &s, const std::vector<std::string> &items);

std::vector<std::string> read_strings_from_stream(std::stringstream &s);

std::stringstream &write_kinds_to_stream(std::stringstream &s, const std::vector<metadata::Kind> &kinds);

std::vector<metadata::Kind> read_kinds_from_stream(std::stringstream &s);

uint32_t size_of_string(const std::string &s);

uint32_t size_of_strings(const std::vector<std::string> &strings);

uint32_t size_of_kind(metadata::Kind kind);

uint32_t size_of_kinds(const std::vector<metadata::Kind> &kinds);

}

#endif //MATCHBOXDB_STREAMUTILS_H
