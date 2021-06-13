#ifndef MATCHBOXDB_NODE_H
#define MATCHBOXDB_NODE_H

#include <ostream>
#include <string>

namespace ast {

class Node {
public:
    virtual std::string token_literal() const = 0;

    virtual std::ostream &repr(std::ostream &os) const = 0;

    friend std::ostream &operator<<(std::ostream &os, const Node& node) {
        return node.repr(os);
    }
};

}

#endif //MATCHBOXDB_NODE_H
