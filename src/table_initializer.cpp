#include "table_initializer.h"
#include "metadata_page.h"
#include "page.h"
#include "slotted_data_page.h"
#include "row_mapping_page.h"

#include <utility>
#include <fstream>
#include <vector>
#include <iostream>

initializers::TableInitializer::TableInitializer(std::string table_name, std::string file_name, uint32_t file_size,
                                                 metadata::Metadata metadata) :
    _metadata{std::move(metadata)},
    _table_name(std::move(table_name)),
    _file_name(std::move(file_name)),
    _file_size(file_size) {
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
    page::MetadataPage metadata_page{
        _table_name,
        _metadata.names,
        _metadata.types,
        calculate_data_pages(),
        calculate_rowmap_pages(),
        0,
    };
    auto buffer = metadata_page.buffer();

    ofs.seekp(0, std::ios::beg);
    ofs.write(reinterpret_cast<const char *>(buffer.data()), page::k_page_size);
    return ofs;
}

std::ofstream &initializers::TableInitializer::write_data_pages(std::ofstream &ofs, uint32_t page_count) {
    ofs.seekp(page::k_page_size, std::ios::beg);

    for (auto i = 0; i < page_count; ++i) {
        page::SlottedDataPage slotted_data_page{page::k_header_size, static_cast<uint32_t>(i), page::k_page_size};
        auto buffer = slotted_data_page.empty_page();
        ofs.write(reinterpret_cast<const char *>(buffer.data()), page::k_page_size);
    }

    return ofs;
}

std::ofstream &initializers::TableInitializer::write_rowmap_pages(std::ofstream &ofs, uint32_t page_count) {
    ofs.seekp(0, std::ios::end);

    uint32_t end = ofs.tellp();

    for (auto i = 0; i < page_count; ++i) {
        ofs.seekp(end - page::k_page_size * (i + 1), std::ios::beg);
        page::RowMappingPage row_mapping_page{page::k_header_size, static_cast<uint32_t>(i), page::k_page_size};
        auto buffer = row_mapping_page.empty_page();
        ofs.write(reinterpret_cast<const char *>(buffer.data()), page::k_page_size);
    }

    return ofs;
}

uint32_t initializers::TableInitializer::calculate_rowmap_pages() const {
    auto page_count = calculate_total_pages();
    return static_cast<uint32_t>(std::ceil2(page_count / 100));
}

uint32_t initializers::TableInitializer::calculate_data_pages() const {
    auto page_count = calculate_total_pages();
    return static_cast<uint32_t>(std::ceil2(page_count / 5));
}

uint32_t initializers::TableInitializer::calculate_total_pages() const {
    return _file_size * 1024 * 1024 / page::k_page_size;
}

void initializers::TableInitializer::initialize() {
    auto ofs = initialize_file();
    write_metadata_page(ofs);
    write_data_pages(ofs, calculate_data_pages());
    write_rowmap_pages(ofs, calculate_rowmap_pages());
}
