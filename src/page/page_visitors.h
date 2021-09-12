#ifndef MATCHBOXDB_PAGE_VISITORS_H
#define MATCHBOXDB_PAGE_VISITORS_H

#include "../storage/streamutils.h"
#include "page.h"

namespace page {

class SlottedDataPage;

class MetadataPage;

}

namespace page_visitors {

class PageVisitor {
public:
    virtual void visit(const stream_utils::ByteBuffer &buffer);

    virtual void visit(const page::SlottedDataPage &data_page) = 0;

    virtual void visit(const page::MetadataPage &metadata_page) = 0;
};

class FreePageCollector : public PageVisitor {
public:
    void visit(const stream_utils::ByteBuffer &buffer) override;

    void visit(const page::SlottedDataPage &data_page) override;

    void visit(const page::MetadataPage &metadata_page) override;

    [[nodiscard]] std::vector<page::FreePageInfo> free_data_pages() const;

protected:
    std::vector<page::FreePageInfo> _free_data_pages{};
};

}

#endif //MATCHBOXDB_PAGE_VISITORS_H
