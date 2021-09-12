#include "tuple.h"
#include "metadata.h"
#include "streamutils.h"

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

    _data = stream_utils::buffer_from_stream(s);
}

tuple::Tuple::Tuple(const std::vector<unsigned char> &buffer, const metadata::Metadata &metadata) {
    _data = buffer;
    auto s = stream_utils::build_binary_stream();
    stream_utils::load_stream_from_buffer(s, buffer);

    for (const auto &type: metadata.types) {
        _attributes.push_back(deserialize(s, type));
    }
}

std::ostream &tuple::operator<<(std::ostream &os, const tuple::Tuple &t) {
    OstreamVisitor v{os};
    for (const auto &attribute: t.attributes()) {
        std::visit(v, attribute);
    }
    return os;
}

uint32_t tuple::Tuple::size() const {
    return _data.size();
}

const std::vector<metadata::DataType> &tuple::Tuple::attributes() const {
    return _attributes;
}

bool tuple::Tuple::operator==(const tuple::Tuple &rhs) const {
    return _attributes == rhs._attributes &&
           _data == rhs._data;
}

bool tuple::Tuple::operator!=(const tuple::Tuple &rhs) const {
    return !(rhs == *this);
}

uint32_t tuple::Tuple::storage_size_required_for_tuple() const {
    return size() + (2 * sizeof(uint32_t));
}

metadata::DataType tuple::deserialize(std::stringstream &s, metadata::Kind kind) {
    switch (kind) {
        case metadata::Kind::String: {
            return stream_utils::read_string_from_stream(s);
        }

        case metadata::Kind::UnsignedInt: {
            return stream_utils::read_data_from_stream<uint32_t>(s);
        }

        case metadata::Kind::Boolean: {
            auto data = stream_utils::read_data_from_stream<unsigned char>(s);
            return data != 0;
        }

        default:
            throw std::invalid_argument{"invalid kind"};
    }
}

std::ostream &tuple::operator<<(std::ostream &os, const tuple::TupleView &tuple_view) {
    OstreamVisitor v{os};
    for (const auto &attribute: tuple_view.values()) {
        std::visit(v, attribute);
    }
    return os;
}

tuple::SerializationVisitor::SerializationVisitor(std::stringstream &s) : s(s) {}

void tuple::SerializationVisitor::operator()(const std::string &data) {
    stream_utils::write_string_to_stream(s, data);
}

void tuple::SerializationVisitor::operator()(uint32_t data) {
    stream_utils::write_data_to_stream(s, data);
}

void tuple::SerializationVisitor::operator()(bool data) {
    unsigned char bool_as_int = data ? 1 : 0;
    stream_utils::write_data_to_stream(s, bool_as_int);
}

tuple::OstreamVisitor::OstreamVisitor(std::ostream &s) : s{s} {
}

void tuple::OstreamVisitor::operator()(const std::string &data) {
    s << "\t\t" << data << "\t\t";
}

void tuple::OstreamVisitor::operator()(uint32_t data) {
    s << "\t\t" << data << "\t\t";
}

void tuple::OstreamVisitor::operator()(bool data) {
    s << "\t\t" << std::boolalpha << data << "\t\t";
}

tuple::TupleView::TupleView(metadata::Metadata metadata, tuple::Tuple tuple,
                            std::optional<std::unordered_set<std::string>> columns) : _metadata(std::move(metadata)),
                                                                                      _tuple(std::move(tuple)),
                                                                                      _columns(std::move(columns)) {
    for (auto i = 0; i < _metadata.names.size(); ++i) {
        _attribute_view[_metadata.names[i]] = _tuple.attributes()[i];
    }
}

metadata::DataType tuple::TupleView::operator[](const std::string &key) const {
    return _attribute_view.at(key);
}

std::vector<metadata::DataType> tuple::TupleView::values() const {
    if (!_columns) {
        return _tuple.attributes();
    }

    std::vector<metadata::DataType> values{};
    for (const auto &name: _metadata.names) {
        if (_columns->contains(name)) {
            values.push_back(_attribute_view.at(name));
        }
    }

    return values;
}

std::string tuple::TupleView::json(bool pretty) const {
    std::stringstream ss;
    ss << "{";
    if (pretty) {
        ss << "\n";
    }


    auto names_size = _metadata.names.size();
    auto column_count = _columns ? _columns->size() : names_size;

    for (auto i = 0; i < names_size; ++i) {

        auto name = _metadata.names[i];
        auto value = _tuple.attributes()[i];
        auto kind = _metadata.types[i];

        if (_columns->contains(name)) {
            if (pretty) {
                ss << "  ";
            }

            ss << '"' << name << '"';
            if (pretty) {
                ss << ": ";
            } else {
                ss << ":";
            }

            switch (kind) {
                case metadata::Kind::String:
                    ss << '"' << std::get<std::string>(value) << '"';
                    break;
                case metadata::Kind::UnsignedInt:
                    ss << std::get<uint32_t>(value);
                    break;
                case metadata::Kind::Boolean:
                    ss << std::boolalpha << std::get<bool>(value);
                    break;
            }

            if (i < column_count - 1) {
                ss << ',';
            }

            if (pretty) {
                ss << "\n";
            }
        }
    }

    if (pretty) {
        ss << "\n";
    }

    ss << "}";
    return ss.str();
}
