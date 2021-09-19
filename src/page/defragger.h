#ifndef MATCHBOXDB_DEFRAGGER_H
#define MATCHBOXDB_DEFRAGGER_H

#include <vector>
#include <atomic>
#include "slotted_data_page.h"

namespace page_cache {
class PageCache;
}

namespace page {

using DefragList = std::vector<SlottedDataPage>;

class PageDefragger {
public:
    explicit PageDefragger(page_cache::PageCache &page_cache);

    void add_page_to_defrag_list(const page::SlottedDataPage* page, const std::string &table_name);

    void operator()();

    void request_stop();

protected:
    page_cache::PageCache &_page_cache;
    std::unordered_map<std::string, DefragList> _defrag_lists{};
    std::atomic<bool> _stop_requested{false};
};
}

#endif //MATCHBOXDB_DEFRAGGER_H
