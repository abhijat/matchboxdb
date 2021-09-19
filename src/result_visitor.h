#ifndef MATCHBOXDB_RESULT_VISITOR_H
#define MATCHBOXDB_RESULT_VISITOR_H

#include <iosfwd>
#include <cstdint>
#include <vector>
#include "storage/tuple.h"

namespace command_executor {
class EmptyResult;
}

namespace presentation {
struct ResultVisitor {
    explicit ResultVisitor(std::ostream &os);

    void operator()(uint32_t n);

    void operator()(const std::vector<tuple::TupleView> &tuples);

    void operator()(const command_executor::EmptyResult &empty_result);

    void operator()(const std::string &s);

    std::ostream &os;
};
}

#endif //MATCHBOXDB_RESULT_VISITOR_H
