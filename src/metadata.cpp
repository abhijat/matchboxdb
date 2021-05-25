#include "metadata.h"

metadata::Metadata::Metadata(std::vector<std::string> names, std::vector<metadata::Kind> types) :
    names(std::move(names)),
    types(std::move(types)) {}
