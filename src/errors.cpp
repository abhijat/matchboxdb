#include "errors.h"

#include <utility>
#include <sstream>

const char *errors::TableDoesNotExist::what() const noexcept {
    return _what.c_str();
}

errors::TableDoesNotExist::TableDoesNotExist(std::string tablename) : _tablename(std::move(tablename)) {
    std::stringstream ss;
    ss << "table " << _tablename << " does not exist";
    _what = ss.str();
}
