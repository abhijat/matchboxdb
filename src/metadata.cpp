#include "metadata.h"

metadata::Metadata::Metadata(std::vector<std::string> names, std::vector<metadata::Kind> types) :
    names(std::move(names)),
    types(std::move(types)) {}

std::ostream &metadata::operator<<(std::ostream &os, metadata::Kind kind) {
    switch (kind) {
        case Kind::String:
            os << "String";
            break;
        case Kind::UnsignedInt:
            os << "UnsignedInt";
            break;
        case Kind::Boolean:
            os << "Boolean";
            break;
    }
    return os;
}
