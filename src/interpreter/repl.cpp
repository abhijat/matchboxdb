#include <iostream>
#include "lexer.h"


void loop(std::istream &in, std::ostream &out) {
    out << "type some SQL\n";
    while (true) {
        out << ">> ";

        std::string input;
        in >> input;

        std::getline(in, input);

        if (!in) {
            out << "Bye\n";
            return;
        }

        lexer::Lexer l{input};

        auto t = l.next_token();
        while (t.kind != token::TokenKind::ENDOFINPUT) {
            out << t << "\n";
            t = l.next_token();
        }
    }
}

int main() {
    loop(std::cin, std::cout);
}
