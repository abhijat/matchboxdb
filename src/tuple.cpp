#include "tuple.h"
#include "metadata.h"

#include <utility>
#include <sstream>

std::vector<unsigned char> tuple::Tuple::data() const {
    return _data;
}

tuple::Tuple::Tuple(std::vector<metadata::DataType> attrs) : _attributes(std::move(attrs)) {
    std::stringstream s{std::ios::binary | std::ios::in | std::ios::out};

    SerializationVisitor visitor{s};
    for (const auto &attribute: _attributes) {
        std::visit(visitor, attribute);
    }

    auto buffer = s.str();
    _data = std::vector<unsigned char>{buffer.data(), buffer.data() + buffer.size()};
}

tuple::Tuple::Tuple(const std::vector<unsigned char> &buffer, const metadata::Metadata &metadata) {
    _data = buffer;
    std::stringstream s{std::ios::binary | std::ios::in | std::ios::out};
    s.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());

    for (const auto &type: metadata.types) {
        _attributes.push_back(deserialize(s, type));
    }
}

std::ostream &tuple::operator<<(std::ostream &os, const tuple::Tuple &t) {
    OstreamVisitor v{os};
    for (const auto &attribute: t._attributes) {
        std::visit(v, attribute);
    }
    return os;
}

uint32_t tuple::Tuple::size() const {
    return _data.size();
}

metadata::DataType tuple::deserialize(std::stringstream &s, metadata::Kind kind) {
    switch (kind) {
        case metadata::Kind::String: {
            uint32_t size;
            s.read(reinterpret_cast<char *>(&size), sizeof(size));
            char buf[size];
            s.read(buf, size);
            return std::string{buf, size};
        }

        case metadata::Kind::UnsignedInt: {
            uint32_t data;
            s.read(reinterpret_cast<char *>(&data), sizeof(data));
            return data;
        }

        case metadata::Kind::Boolean: {
            unsigned char data;
            s.read(reinterpret_cast<char *>(&data), sizeof(data));
            return data != 0;
        }

        default:
            throw std::invalid_argument{"invalid kind"};
    }
}

tuple::SerializationVisitor::SerializationVisitor(std::stringstream &s) : s(s) {}

void tuple::SerializationVisitor::operator()(const std::string &data) {
    uint32_t data_len = data.size();
    s.write(reinterpret_cast<const char *>(&data_len), sizeof(data_len));
    s.write(data.c_str(), data_len);
}

void tuple::SerializationVisitor::operator()(uint32_t data) {
    s.write(reinterpret_cast<const char *>(&data), sizeof(data));
}

void tuple::SerializationVisitor::operator()(bool data) {
    unsigned char bool_as_int = data ? 1 : 0;
    s.write(reinterpret_cast<const char *>(&bool_as_int), sizeof(bool_as_int));
}

tuple::OstreamVisitor::OstreamVisitor(std::ostream &s) : s{s} {
}

void tuple::OstreamVisitor::operator()(const std::string &data) {
    s << " [" << data << "] ";
}

void tuple::OstreamVisitor::operator()(uint32_t data) {
    s << " [" << data << "] ";
}

void tuple::OstreamVisitor::operator()(bool data) {
    s << " [" << std::boolalpha << data << "] ";
}

