#ifndef MATCHBOXDB_TUPLE_H
#define MATCHBOXDB_TUPLE_H

#include <variant>
#include <string>
#include <vector>
#include <ostream>
#include "metadata.h"

namespace tuple {


class Tuple {
public:
    explicit Tuple(std::vector<metadata::DataType> attrs);

    Tuple(const std::vector<unsigned char> &buffer, const metadata::Metadata &metadata);

    [[nodiscard]] std::vector<unsigned char> data() const;

    [[nodiscard]] uint32_t size() const;

    bool operator==(const Tuple &rhs) const;

    bool operator!=(const Tuple &rhs) const;

    friend std::ostream &operator<<(std::ostream &os, const Tuple &t);

    [[nodiscard]] const std::vector<metadata::DataType> &attributes() const;

protected:
    std::vector<metadata::DataType> _attributes;
    std::vector<unsigned char> _data;
};

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


#endif //MATCHBOXDB_TUPLE_H
