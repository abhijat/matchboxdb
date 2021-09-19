#ifndef MATCHBOXDB_DESCRIBE_ACTION_H
#define MATCHBOXDB_DESCRIBE_ACTION_H

#include <string>

namespace ast {
class DescribeStatement;
}

namespace page_cache {
class PageCache;
}

namespace actions {
class DescribeAction {
public:
    explicit DescribeAction(page_cache::PageCache &page_cache,
                            const ast::DescribeStatement &describe_statement);

    [[nodiscard]] std::string describe();

protected:
    page_cache::PageCache &_page_cache;
    const ast::DescribeStatement &_describe_statement;

    std::string describe_table(const std::string& table_name);

    std::string describe_all_tables();
};
}

#endif //MATCHBOXDB_DESCRIBE_ACTION_H
