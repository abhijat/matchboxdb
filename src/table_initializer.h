#ifndef MATCHBOXDB_TABLE_INITIALIZER_H
#define MATCHBOXDB_TABLE_INITIALIZER_H

#include <string>
#include "metadata.h"

namespace initializers {

class TableInitializer {
public:
    TableInitializer(std::string table_name, std::string file_name, uint32_t file_size, metadata::Metadata metadata);

    void initialize();

protected:
    std::ofstream initialize_file();

    std::ofstream &write_metadata_page(std::ofstream &ofs);

    [[nodiscard]] uint32_t calculate_total_pages() const;

protected:
    metadata::Metadata _metadata;
    std::string _table_name;
    std::string _file_name;
    uint32_t _file_size;
};

}


#endif //MATCHBOXDB_TABLE_INITIALIZER_H
