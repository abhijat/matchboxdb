#include "object.h"

objects::Integer::Integer(int64_t value) : _value(value) {}

int64_t objects::Integer::value() const {
    return _value;
}

objects::Kind objects::Integer::kind() const {
    return Kind::Integer;
}

std::string objects::Integer::inspect() const {
    return std::to_string(_value);
}

objects::Boolean::Boolean(bool value) : _value(value) {}

bool objects::Boolean::value() const {
    return _value;
}

objects::Kind objects::Boolean::kind() const {
    return Kind::Boolean;
}

std::string objects::Boolean::inspect() const {
    return _value ? "true" : "false";
}

objects::Kind objects::Null::kind() const {
    return Kind::Null;
}

std::string objects::Null::inspect() const {
    return "NULL";
}
