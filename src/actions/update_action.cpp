#include "update_action.h"

#include "../sql_parser/update_statement.h"
#include "../page/page_cache.h"
#include "../sql_parser/expression.h"
#include "../tuple_filter.h"
#include "../sql_parser/expression_visitor.h"
#include "../insert_object.h"

actions::UpdateAction::UpdateAction(page_cache::PageCache &page_cache, const ast::UpdateStatement &update_statement)
    : _page_cache(page_cache), _update_statement(update_statement) {
    if (_update_statement.where()) {
        _where = _update_statement.where()->get();
    } else {
        _where = nullptr;
    }

}

uint32_t actions::UpdateAction::update() {
    auto table_name = _update_statement.table().table_name();
    auto metadata = _page_cache.metadata_for_table(table_name);

    uint32_t updated_pages{};
    for (auto page: _page_cache.enumerate_pages(table_name, page::PageType::Data)) {
        auto data_page = dynamic_cast<page::SlottedDataPage *>(page);
        for (auto &&tuple_and_slot: data_page->enumerate_tuples()) {
            auto tuple = tuple::Tuple{tuple_and_slot.byte_buffer, metadata};
            if (tuple_filter::matches(tuple, metadata, _where)) {
                // TODO there are several optimizations to do here.
                //  we can put the tuple at the same slot if the size is the same
                //  if the new tuple is smaller, it can result in gaps in the page
                data_page->delete_tuple_at_slot_id(tuple_and_slot.slot_id);
                _page_cache.mark_page_dirty(table_name, data_page);
                store_updated_tuple(table_name, metadata, data_page, tuple);
                updated_pages += 1;
            }
        }
    }

    return updated_pages;
}

void actions::UpdateAction::store_updated_tuple(const std::string &table_name, const metadata::Metadata &metadata,
                                                page::SlottedDataPage *data_page, const tuple::Tuple &tuple) const {
    auto new_tuple = updated_tuple(tuple, metadata);
    uint32_t size_required_for_tuple = new_tuple.size() + (2 * sizeof(uint32_t));

    if (size_required_for_tuple < data_page->free_space()) {
        data_page->store_tuple(new_tuple);
    } else {
        InsertObject{_page_cache, table_name, new_tuple}.save();
    }
}

tuple::Tuple actions::UpdateAction::updated_tuple(const tuple::Tuple &tuple, const metadata::Metadata &metadata) const {
    auto updates = generate_updates(tuple, metadata);

    const auto &attributes = tuple.attributes();

    std::vector<metadata::DataType> updated_attributes(attributes.size());

    for (auto i = 0; i < updated_attributes.size(); ++i) {
        auto field_name = metadata.names[i];
        if (updates.contains(field_name)) {
            updated_attributes[i] = updates[field_name];
        } else {
            updated_attributes[i] = attributes[i];
        }
    }

    return tuple::Tuple{updated_attributes};
}

std::unordered_map<std::string, metadata::DataType>
actions::UpdateAction::generate_updates(const tuple::Tuple &tuple, const metadata::Metadata &metadata) const {
    std::unordered_map<std::string, metadata::DataType> updates{};

    ast::EvaluatingExpressionVisitor visitor{tuple, metadata};
    for (const auto&[identifier, expression]: _update_statement.update()) {
        updates[identifier.value()] = *visitor.evaluate(*expression);
    }

    return updates;
}
