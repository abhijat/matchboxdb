#ifndef MATCHBOXDB_FILE_WRITE_H
#define MATCHBOXDB_FILE_WRITE_H

#include <string>
#include <fstream>

namespace storage {

void create_empty_file_of_size(const std::string &path, size_t size_in_mb);

void write_string(std::fstream &f, const std::string &data);

void write_int(std::fstream &f, uint32_t data);

void write_page_directory(std::fstream &f, size_t num_pages);

void initialize_database(const std::string &path);

void write_page_at_offset(std::fstream &f, uint32_t offset, std::stringstream &data);

std::stringstream build_test_page(uint32_t test_data);

}

#endif //MATCHBOXDB_FILE_WRITE_H
