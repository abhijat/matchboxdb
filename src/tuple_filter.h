#ifndef MATCHBOXDB_TUPLE_FILTER_H
#define MATCHBOXDB_TUPLE_FILTER_H

namespace tuple {
class Tuple;
}

namespace metadata {
class Metadata;
}

namespace ast {
class Expression;
}

namespace tuple_filter {

bool matches(const tuple::Tuple &tuple, const metadata::Metadata &metadata, const ast::Expression *where);

}

#endif //MATCHBOXDB_TUPLE_FILTER_H
