#ifndef MATCHBOXDB_NODE_H
#define MATCHBOXDB_NODE_H

#include <string>

namespace ast {

class Node {
public:
    virtual std::string token_literal() const = 0;
};

}

#endif //MATCHBOXDB_NODE_H
