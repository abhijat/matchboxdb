#include "utils.h"

std::string storage_utils::file_name_from_table_name(const std::string &table_name) {
    // TODO - this needs to be in a user definable path, not PWD
    return table_name + ".mbx";
}
