#include <iostream>

#include "command_executor.h"
#include "page/page_cache.h"
#include "result_visitor.h"
#include "sql_parser/parser.h"
#include "sql_parser/statement.h"

#include "dependencies/linenoise/linenoise.h"


void tab_completion(const char *input, linenoiseCompletions *completions) {
    std::string s{input};

    std::string prefix{};
    std::string final_term{};

    auto pos = s.find_last_of(' ');
    if (pos == std::string::npos) {
        prefix = "";
        final_term = s;
    } else {
        prefix = s.substr(0, pos);
        final_term = s.substr(pos + 1);
    }

    for (const auto &[k, v]: token::k_keywords) {
        if (k.starts_with(final_term)) {
            std::stringstream ss;

            if (!prefix.empty()) {
                ss << prefix << " ";
            }

            ss << k << " ";
            linenoiseAddCompletion(completions, ss.str().c_str());
        }
    }
}

void repl(std::ostream &os, std::istream &is) {
    linenoiseHistorySetMaxLen(100);
    linenoiseHistoryLoad(".matchboxdb_history");
    linenoiseSetCompletionCallback(tab_completion);

    auto cache = page_cache::PageCache::scan_and_build_page_cache(60, ".");
    command_executor::CommandExecutor executor{cache};

    presentation::ResultVisitor result_visitor{os};

    while (true) {
        auto line_read = linenoise(">> ");

        if (!line_read) {
            linenoiseHistorySave(".matchboxdb_history");

            os << "Bye!\n";
            return;
        }

        std::string line{line_read};
        if (!line.empty()) {
            const auto &statement = parser::Parser{lexer::Lexer{line_read}}.parse();
            auto result = executor.execute(*statement);

            std::visit(result_visitor, result);

            linenoiseHistoryAdd(line_read);
            linenoiseFree(line_read);
        }
    }
}

int main() {
    repl(std::cout, std::cin);
}
