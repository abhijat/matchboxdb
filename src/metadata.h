#ifndef MATCHBOXDB_METADATA_H
#define MATCHBOXDB_METADATA_H

#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace metadata {
using DataType = std::variant<std::string, uint32_t, bool>;

enum class Kind {
    String,
    UnsignedInt,
    Boolean,
};

std::ostream &operator<<(std::ostream &os, Kind kind);

struct Metadata {
    Metadata(std::vector<std::string> names, std::vector<metadata::Kind> types);

    std::vector<std::string> names;
    std::vector<metadata::Kind> types;
};

}

#endif // MATCHBOXDB_METADATA_H
