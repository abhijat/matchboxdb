#ifndef MATCHBOXDB_UTILS_H
#define MATCHBOXDB_UTILS_H

#include <string>
#include <vector>

namespace storage_utils {

static const std::string k_table_file_extension{".mbx"};

std::string file_name_from_table_name(const std::string& table_name);

std::vector<std::string> scan_table_names_from_files(const std::string &data_path_root);

}

#endif //MATCHBOXDB_UTILS_H
