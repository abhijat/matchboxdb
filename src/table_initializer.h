#ifndef MATCHBOXDB_TABLE_INITIALIZER_H
#define MATCHBOXDB_TABLE_INITIALIZER_H

#include <string>
#include "metadata.h"
#include "sql_parser/create_statement.h"

namespace ast {
class CreateStatement;
}

namespace initializers {

class TableInitializer {
public:
    TableInitializer(const ast::CreateStatement &create_statement, uint32_t file_size);

    void initialize();

protected:
    std::ofstream initialize_file();

    std::ofstream &write_metadata_page(std::ofstream &ofs);

    [[nodiscard]] uint32_t calculate_total_pages() const;

protected:
    std::string _file_name;
    std::string _table_name;
    metadata::Metadata _metadata;
    uint32_t _file_size;
};

}


#endif //MATCHBOXDB_TABLE_INITIALIZER_H
