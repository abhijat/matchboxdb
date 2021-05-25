#ifndef MATCHBOXDB_METADATA_H
#define MATCHBOXDB_METADATA_H

#include <variant>
#include <utility>
#include <sstream>
#include <string>
#include <vector>
#include <ostream>

namespace metadata {
using DataType = std::variant<std::string, uint32_t, bool>;

enum class Kind {
    String,
    UnsignedInt,
    Boolean,
};

struct Metadata {
    Metadata(std::vector<std::string> names, std::vector<metadata::Kind> types);
    std::vector<std::string> names;
    std::vector<metadata::Kind> types;
};

}

#endif //MATCHBOXDB_METADATA_H
