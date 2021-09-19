#include "result_visitor.h"

presentation::ResultVisitor::ResultVisitor(std::ostream &os) : os{os} {
}

void presentation::ResultVisitor::operator()(uint32_t n) {
    os << n << "\n";
}

void presentation::ResultVisitor::operator()(const std::vector<tuple::TupleView> &tuples) {
    for (const auto &t: tuples) {
        os << t.json() << ",\n";
    }

    os << tuples.size() << " rows returned\n";
}

void presentation::ResultVisitor::operator()(const command_executor::EmptyResult &empty_result) {
    os << "\n";
}

void presentation::ResultVisitor::operator()(const std::string &s) {
    os << "\n";
}
