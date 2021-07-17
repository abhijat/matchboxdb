#include <iostream>
#include "sql_parser/lexer.h"

void repl(std::ostream &os, std::istream &is) {
    while (true) {
        os << ">> ";

        std::string line;
        std::getline(is, line);

        if (!is) {
            os << "Bye!\n";
            return;
        }

        lexer::Lexer lexer{line};
        for (auto t = lexer.next_token(); t.kind() != token::Kind::EndOfInput; t = lexer.next_token()) {
            os << t << "\n";
        }
    }
}

int main() {
    repl(std::cout, std::cin);
}
