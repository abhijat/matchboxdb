#include "table_initializer.h"
#include "metadata_page.h"
#include "page.h"
#include "sql_parser/create_statement.h"

#include <fstream>
#include <vector>
#include <iostream>

initializers::TableInitializer::TableInitializer(const ast::CreateStatement &create_statement, uint32_t file_size) :
    _file_size(file_size), _metadata(create_statement.metadata()),
    _table_name(create_statement.table_name().table_name()) {
    _file_name = _table_name + ".mbx";
}

std::ofstream initializers::TableInitializer::initialize_file() {
    std::ofstream db{_file_name, std::ios::binary | std::ios::trunc};
    if (!db) {
        throw std::ios::failure("Failed to open table file for initialization");
    }

    std::vector<unsigned char> null_bytes(_file_size * 1024 * 1024);
    db.write(reinterpret_cast<const char *>(null_bytes.data()), static_cast<uint32_t>(null_bytes.size()));
    return db;
}

std::ofstream &initializers::TableInitializer::write_metadata_page(std::ofstream &ofs) {
    page::MetadataPage metadata_page{_table_name, _metadata.names, _metadata.types, 0, 0, 0, calculate_total_pages()};
    ofs.seekp(0, std::ios::beg);
    ofs.write(reinterpret_cast<const char *>(metadata_page.buffer().data()), page::k_page_size);
    return ofs;
}

uint32_t initializers::TableInitializer::calculate_total_pages() const {
    return _file_size * 1024 * 1024 / page::k_page_size;
}

void initializers::TableInitializer::initialize() {
    auto ofs = initialize_file();
    write_metadata_page(ofs);
}
