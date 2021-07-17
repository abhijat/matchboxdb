#include "node.h"

std::ostream &ast::operator<<(std::ostream &os, const ast::Node &node) {
    node.repr(os);
    return os;
}
