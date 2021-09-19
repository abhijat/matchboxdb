#include <iomanip>
#include <iterator>
#include "describe_action.h"

#include "../sql_parser/describe_statement.h"
#include "../page/page_cache.h"

actions::DescribeAction::DescribeAction(page_cache::PageCache &page_cache,
                                        const ast::DescribeStatement &describe_statement) : _describe_statement(
    describe_statement), _page_cache(page_cache) {}

std::string actions::DescribeAction::describe() {
    if (_describe_statement.table()) {
        return describe_table(_describe_statement.table()->table_name());
    } else {
        return describe_all_tables();
    }
}

std::string actions::DescribeAction::describe_table(const std::string &table_name) {
    auto metadata = _page_cache.metadata_for_table(table_name);
    std::stringstream ss{};
    ss << table_name << " (\n";

    // TODO calculate max width first
    for (auto i = 0; i < metadata.names.size(); ++i) {
        ss << ' ' << std::left << std::setw(24) << metadata.names[i] << std::setw(16) << metadata.types[i] << "\n";
    }

    ss << ")\n";
    return ss.str();
}

std::string actions::DescribeAction::describe_all_tables() {

    std::vector<std::string> sorted_tables{_page_cache.table_names()};

    std::sort(sorted_tables.begin(),  sorted_tables.end());

    std::vector<std::string> descriptions{sorted_tables.size()};

    std::transform(sorted_tables.cbegin(), sorted_tables.cend(), descriptions.begin(), [&](const auto &table_name) {
        return describe_table(table_name);
    });

    std::stringstream ss{};
    std::copy(descriptions.cbegin(), descriptions.cend(), std::ostream_iterator<std::string>(ss, "\n"));
    return ss.str();
}
