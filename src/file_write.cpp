#include <iostream>
#include <vector>

#include "file_write.h"
#include "file_read.h"

void storage::create_empty_file_of_size(const std::string &path, size_t size_in_mb) {
    std::fstream db;
    db.open(path, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc);

    if (!db.is_open()) {
        throw std::runtime_error{"Unable to open file for initial data setup"};
    }

    std::vector<char> one_kb(1024, 0);
    for (int i = 0; i < 1024 * size_in_mb; ++i) {
        db.write(&one_kb[0], one_kb.size());
    }
}

void storage::write_int(std::fstream &f, uint32_t data) {
    f.write(reinterpret_cast<char *>(&data), sizeof(data));
    if (!f) {
        throw std::runtime_error{"Failed to write INT to file"};
    }
}

void storage::write_string(std::fstream &f, const std::string &data) {
    uint32_t name_size = data.size();
    write_int(f, name_size);
    f.write(data.c_str(), name_size);
    if (!f) {
        throw std::runtime_error{"Failed to write STR to file"};
    }
}

void storage::write_page_directory(std::fstream &f, size_t num_pages) {
    std::string dbname{"MatchBox DB"};

    write_string(f, dbname);

    uint32_t data_pages = num_pages - 1;
    write_int(f, data_pages);

    uint32_t offset = 4096;
    for (uint32_t page_id = 0; page_id < data_pages; ++page_id) {
        write_int(f, page_id);
        write_int(f, offset);
        if (page_id == 100) {
            auto test_page = build_test_page(1234);
            write_page_at_offset(f, offset, test_page);
        }
        offset += 4096;
    }
}

void storage::initialize_database(const std::string &path) {
    std::fstream db;
    db.open(path, std::ios::in | std::ios::out | std::ios::binary);

    if (!db.is_open()) {
        throw std::runtime_error{"Unable to open file for initialization"};
    }

    auto size_in_bytes = file_size_in_bytes(db);

    size_t page_size = 4 * 1024;
    auto number_of_pages = size_in_bytes / page_size;
    write_page_directory(db, number_of_pages);
}

void storage::write_page_at_offset(std::fstream &f, uint32_t offset, std::stringstream &data) {
    f.seekp(offset, std::ios::beg);
    f << data.rdbuf();
    if (!f) {
        std::cout << "Failed to write page at offset " << offset << "\n";
    }
}

std::stringstream storage::build_test_page(uint32_t test_data) {
    std::stringstream page{std::ios::binary | std::ios::in | std::ios::out};
    auto write_count = 4096 / sizeof(test_data);
    for (int i = 0; i < write_count; ++i) {
        page.write(reinterpret_cast<char *>(&test_data), sizeof(test_data));
        if (!page) {
            throw std::runtime_error{"failed to write test data"};
        }
    }
    return page;
}
