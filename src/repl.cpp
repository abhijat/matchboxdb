#include <iostream>
#include "page/page_cache.h"
#include "command_executor.h"
#include "sql_parser/parser.h"
#include "sql_parser/statement.h"

struct ResultVisitor {
    explicit ResultVisitor(std::ostream &os);

    void operator()(uint32_t n);

    void operator()(const std::vector<tuple::TupleView> &tuples);

    std::ostream &os;
};

ResultVisitor::ResultVisitor(std::ostream &os) : os{os} {
}

void ResultVisitor::operator()(uint32_t n) {
    os << n << "\n";
}

void ResultVisitor::operator()(const std::vector<tuple::TupleView> &tuples) {
    for (const auto &t: tuples) {
        os << t << "\n";
    }

    os << tuples.size() << " rows returned\n";
}

void repl(std::ostream &os, std::istream &is) {
    auto cache = page_cache::PageCache::scan_and_build_page_cache(60, ".");
    command_executor::CommandExecutor executor{cache};

    ResultVisitor result_visitor{os};

    while (true) {
        os << ">> ";

        std::string line;
        std::getline(is, line);

        if (!is) {
            os << "Bye!\n";
            return;
        }

        const auto &statement = parser::Parser{lexer::Lexer{line}}.parse();
        auto result = executor.execute(*statement);

        std::visit(result_visitor, result);
    }
}

int main() {
    repl(std::cout, std::cin);
}
