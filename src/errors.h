#ifndef MATCHBOXDB_ERRORS_H
#define MATCHBOXDB_ERRORS_H

#include <exception>
#include <string>

namespace errors {

class TableDoesNotExist : std::exception {

public:
    explicit TableDoesNotExist(std::string tablename);

    [[nodiscard]] const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
protected:
    std::string _tablename;
    std::string _what;
};

}

#endif //MATCHBOXDB_ERRORS_H
