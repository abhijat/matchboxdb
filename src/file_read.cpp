#include <cstring>
#include <iostream>

#include "file_read.h"


size_t storage::file_size_in_bytes(std::fstream &f) {
    auto start = f.tellp();
    f.seekp(0, std::ios::end);
    auto end = f.tellp();
    f.seekp(0, std::ios::beg);
    return end - start;
}

void storage::read_page_directory(std::ifstream &f) {
    auto page_directory = load_page(f, 0);

    auto dbname = read_string(page_directory);
    std::cout << "db name [" << dbname << "]\n";

    auto data_page_count = read_int(page_directory);
    std::cout << "data_page_count " << data_page_count << "\n";
    for (auto i = 0; i < data_page_count; ++i) {
        auto page_id = read_int(page_directory);
        auto page_offset = read_int(page_directory);
        if (page_id == 100) {
            auto page = load_page(f, page_offset);
            read_test_page(page);
        }
    }
}

void storage::read_test_page(std::stringstream &page) {
    for (auto i = 0; i < 1024; ++i) {
        auto t = read_int(page);
        if (t != 1234) {
            throw std::logic_error{"test page read failed, bad data"};
        }
    }
}

std::string storage::read_string(std::stringstream &page) {
    uint32_t string_size = read_int(page);
    char data[string_size];
    page.read(reinterpret_cast<char *>(data), string_size);
    return std::string{data, string_size};
}

uint32_t storage::read_int(std::stringstream &page) {
    uint32_t data;
    page.read(reinterpret_cast<char *>(&data), sizeof(data));
    if (!page) {
        throw std::runtime_error{"failed to read INT from page"};
    }
    return data;
}

std::stringstream storage::load_page(std::ifstream &f, uint32_t offset) {
    f.seekg(offset, std::ios::beg);

    char page_buffer[4096];
    f.read(page_buffer, 4096);
    if (!f) {
        throw std::runtime_error{"failed to read page data from disk"};
    }

    std::stringstream page{std::ios::binary | std::ios::in | std::ios::out};
    page.write(page_buffer, 4096);
    if (!page) {
        throw std::runtime_error{"failed to write page data to _stream"};
    }

    return page;
}
