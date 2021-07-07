#ifndef MATCHBOXDB_OBJECT_H
#define MATCHBOXDB_OBJECT_H

#include <string>
#include <memory>

namespace objects {

enum class Kind {
    Integer,
    Boolean,
    Null,
};

class Visitor;

class Object {
public:
    [[nodiscard]] virtual Kind kind() const = 0;

    [[nodiscard]] virtual std::string inspect() const = 0;

    virtual std::unique_ptr<Object> visit(Visitor &visitor) const = 0;
};

class Integer : public Object {
public:
    explicit Integer(int64_t value);

    [[nodiscard]] int64_t value() const;

    [[nodiscard]] Kind kind() const override;

    [[nodiscard]] std::string inspect() const override;

    std::unique_ptr<Object> visit(Visitor &visitor) const override;

protected:
    int64_t _value;
};

class Boolean : public Object {
public:
    explicit Boolean(bool value);

    [[nodiscard]] bool value() const;

    [[nodiscard]] Kind kind() const override;

    [[nodiscard]] std::string inspect() const override;

    std::unique_ptr<Object> visit(Visitor &visitor) const override;

private:
    bool _value;
};

class Null : public Object {
public:
    [[nodiscard]] Kind kind() const override;

    [[nodiscard]] std::string inspect() const override;

    std::unique_ptr<Object> visit(Visitor &visitor) const override;
};

class Visitor {
public:
    [[nodiscard]] virtual std::unique_ptr<Object> visit(const Integer &integer) const = 0;

    [[nodiscard]] virtual std::unique_ptr<Object> visit(const Boolean &boolean) const = 0;

    [[nodiscard]] virtual std::unique_ptr<Object> visit(const Null &nul) const = 0;
};

class BoolNegateVisitor : public Visitor {
public:
    [[nodiscard]] std::unique_ptr<Object> visit(const Integer &integer) const override;

    [[nodiscard]] std::unique_ptr<Object> visit(const Boolean &boolean) const override;

    [[nodiscard]] std::unique_ptr<Object> visit(const Null &nul) const override;
};

class IntNegateVisitor : public Visitor {
public:
    [[nodiscard]] std::unique_ptr<Object> visit(const Integer &integer) const override;

    [[nodiscard]] std::unique_ptr<Object> visit(const Boolean &boolean) const override;

    [[nodiscard]] std::unique_ptr<Object> visit(const Null &nul) const override;
};

}


#endif //MATCHBOXDB_OBJECT_H
