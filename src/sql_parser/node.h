#ifndef MATCHBOXDB_NODE_H
#define MATCHBOXDB_NODE_H

#include <ostream>

namespace ast {

class Node {
public:
    virtual void repr(std::ostream &os) const = 0;
};

std::ostream &operator<<(std::ostream &os, const Node &node);

}

#endif //MATCHBOXDB_NODE_H
