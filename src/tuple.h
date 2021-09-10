#ifndef MATCHBOXDB_TUPLE_H
#define MATCHBOXDB_TUPLE_H

#include "metadata.h"
#include <ostream>
#include <string>
#include <variant>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace tuple {


class Tuple {
public:
    explicit Tuple(std::vector<metadata::DataType> attrs);

    Tuple(const std::vector<unsigned char> &buffer, const metadata::Metadata &metadata);

    [[nodiscard]] std::vector<unsigned char> data() const;

    [[nodiscard]] uint32_t size() const;

    bool operator==(const Tuple &rhs) const;

    bool operator!=(const Tuple &rhs) const;


    [[nodiscard]] const std::vector<metadata::DataType> &attributes() const;

    [[nodiscard]] uint32_t storage_size_required_for_tuple() const;

protected:
    std::vector<metadata::DataType> _attributes;
    std::vector<unsigned char> _data;
};

std::ostream &operator<<(std::ostream &os, const Tuple &t);


class TupleView {
public:
    TupleView(metadata::Metadata metadata, tuple::Tuple tuple,
              std::optional<std::unordered_set<std::string>> columns);

    metadata::DataType operator[](const std::string& key) const;

    std::vector<metadata::DataType> values() const;

protected:
    metadata::Metadata _metadata;
    tuple::Tuple _tuple;
    std::unordered_map<std::string, metadata::DataType> _attribute_view;
    std::optional<std::unordered_set<std::string>> _columns{};
};

std::ostream &operator<<(std::ostream &os, const TupleView &tuple_view);

metadata::DataType deserialize(std::stringstream &s, metadata::Kind kind);

struct SerializationVisitor {
    std::stringstream &s;

    explicit SerializationVisitor(std::stringstream &s);

    void operator()(const std::string &data);

    void operator()(uint32_t data);

    void operator()(bool data);
};

struct OstreamVisitor {
    std::ostream &s;

    explicit OstreamVisitor(std::ostream &s);

    void operator()(const std::string &data);

    void operator()(uint32_t data);

    void operator()(bool data);
};

}

#endif // MATCHBOXDB_TUPLE_H
