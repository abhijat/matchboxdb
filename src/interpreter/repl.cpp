#include <iostream>

#include "lexer.h"
#include "parser.h"
#include "eval.h"
#include "object.h"


void dump_errors(std::ostream &out, const std::vector<std::string> &errors) {
    for (const auto &error : errors) {
        out << "\t" << error << "\n";
    }
}

void loop(std::istream &in, std::ostream &out) {
    out << "type some SQL\n";
    while (true) {
        out << ">> ";

        std::string input;
        std::getline(in, input);

        if (input.empty() && in) {
            continue;
        }

        if (!in) {
            out << "Bye\n";
            return;
        }

        lexer::Lexer l{input};
        parser::Parser p{l};
        auto program = p.parse();

        if (!p.errors().empty()) {
            dump_errors(out, p.errors());
            continue;
        }

        auto evaluation = eval::evaluate(program);
        if (evaluation) {
            out << evaluation->inspect() << "\n";
        }
    }
}

int main() {
    loop(std::cin, std::cout);
}
