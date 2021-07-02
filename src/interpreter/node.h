#ifndef MATCHBOXDB_NODE_H
#define MATCHBOXDB_NODE_H

#include <memory>
#include <ostream>
#include <string>

namespace objects {
class Object;
}

namespace eval {
class Visitor;
}

namespace ast {

class Node {
public:
    [[nodiscard]] virtual std::string token_literal() const = 0;

    virtual std::ostream &repr(std::ostream &os) const = 0;

    [[nodiscard]] virtual std::unique_ptr<objects::Object> visit(eval::Visitor& visitor) const = 0;

    friend std::ostream &operator<<(std::ostream &os, const Node &node) {
        return node.repr(os);
    }
};

}

#endif //MATCHBOXDB_NODE_H
