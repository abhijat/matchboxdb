#ifndef MATCHBOXDB_RESULT_VISITOR_H
#define MATCHBOXDB_RESULT_VISITOR_H

#include <iosfwd>
#include <cstdint>
#include <vector>
#include "storage/tuple.h"

namespace presentation {
struct ResultVisitor {
    explicit ResultVisitor(std::ostream &os);

    void operator()(uint32_t n);

    void operator()(const std::vector<tuple::TupleView> &tuples);

    std::ostream &os;
};
}

#endif //MATCHBOXDB_RESULT_VISITOR_H
