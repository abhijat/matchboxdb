#ifndef MATCHBOXDB_FILE_READ_H
#define MATCHBOXDB_FILE_READ_H

#include <fstream>
#include <sstream>

namespace storage {

size_t file_size_in_bytes(std::fstream &f);

void read_page_directory(std::ifstream &f);

std::string read_string(std::stringstream &page);

uint32_t read_int(std::stringstream &page);

std::stringstream load_page(std::ifstream &f, uint32_t offset);

void read_test_page(std::stringstream &page);
}

#endif //MATCHBOXDB_FILE_READ_H
