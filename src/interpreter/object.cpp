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

std::unique_ptr<objects::Object> objects::Integer::visit(objects::Visitor &visitor) const {
    return visitor.visit(*this);
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

std::unique_ptr<objects::Object> objects::Boolean::visit(objects::Visitor &visitor) const {
    return visitor.visit(*this);
}

objects::Kind objects::Null::kind() const {
    return Kind::Null;
}

std::string objects::Null::inspect() const {
    return "NULL";
}

std::unique_ptr<objects::Object> objects::Null::visit(objects::Visitor &visitor) const {
    return visitor.visit(*this);
}

std::unique_ptr<objects::Object> objects::BoolNegateVisitor::visit(const objects::Integer &integer) const {
    return std::make_unique<objects::Boolean>(integer.value() == 0);
}

std::unique_ptr<objects::Object> objects::BoolNegateVisitor::visit(const objects::Boolean &boolean) const {
    return std::make_unique<objects::Boolean>(!boolean.value());
}

std::unique_ptr<objects::Object> objects::BoolNegateVisitor::visit(const objects::Null &nul) const {
    return std::make_unique<objects::Boolean>(true);
}

std::unique_ptr<objects::Object> objects::IntNegateVisitor::visit(const objects::Integer &integer) const {
    return std::make_unique<objects::Integer>(-integer.value());
}

std::unique_ptr<objects::Object> objects::IntNegateVisitor::visit(const objects::Boolean &boolean) const {
    return nullptr;
}

std::unique_ptr<objects::Object> objects::IntNegateVisitor::visit(const objects::Null &nul) const {
    return nullptr;
}
