#include <thread>
#include "defragger.h"

#include "page_cache.h"
#include "../logging.h"

page::PageDefragger::PageDefragger(page_cache::PageCache &page_cache) : _page_cache(page_cache) {}

void page::PageDefragger::add_page_to_defrag_list(const page::SlottedDataPage *page, const std::string &table_name) {
    // A copy of the original page, if the defrag is successful the original will be replaced in cache
    page::SlottedDataPage slotted_data_page{*page};
    _defrag_lists[table_name].push_back(slotted_data_page);
}

void page::PageDefragger::operator()() {
    while (!_stop_requested.load()) {
        for (auto&[table, list]: _defrag_lists) {
            auto new_versions = DefragList{};
            for (auto &page: list) {
                // expensive defrag operation, done on a copy of the page.
                page.defrag_page(_page_cache.metadata_for_table(table));

                auto page_from_cache = _page_cache.get_page_id(page.page_id(), table, PageType::Data);
                auto data_page = dynamic_cast<page::SlottedDataPage *>(page_from_cache);

                // if the cached page version matches ours, no one changed it since we did the defrag.
                // we are good to write over it.
                {
                    std::lock_guard<std::mutex> guard{data_page->mutex()};
                    if (page.version() == data_page->version()) {
                        log::info("overwriting data page with de-fragmented version: ", page.page_id());
                        data_page->overwrite_with(page);
                        _page_cache.mark_page_dirty(table, data_page, false);
                    } else {
                        // TODO we are locking the data page here, we do not actually need this
                        // cached page has been changed since we defragged. put the page back on the list,
                        // we can try again later. the page put on the list is the latest page from cache.
                        // it will need to be defragged again.
                        log::info("page in defrag list is out of date. adding back to queue: our version:",
                                  page.version(),
                                  "cache version:", data_page->version());
                        new_versions.emplace_back(*data_page);
                    }
                }
            }

            _page_cache.write_dirty_pages(table);

            _defrag_lists[table].clear();
            for (const auto& new_page: new_versions) {
                _defrag_lists[table].push_back(new_page);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
    }

    log::info("page defragger is shutting down!");
}

void page::PageDefragger::request_stop() {
    _stop_requested.store(true);
}
