#ifndef MATCHBOXDB_OBJECT_H
#define MATCHBOXDB_OBJECT_H

#include <string>

namespace objects {

enum class Kind {
    Integer,
    Boolean,
    Null,
};

class Object {
public:
    [[nodiscard]] virtual Kind kind() const = 0;

    [[nodiscard]] virtual std::string inspect() const = 0;
};

class Integer : public Object {
public:
    explicit Integer(int64_t value);

    [[nodiscard]] int64_t value() const;

    [[nodiscard]] Kind kind() const override;

    [[nodiscard]] std::string inspect() const override;

protected:
    int64_t _value;
};

class Boolean : public Object {
public:
    explicit Boolean(bool value);

    [[nodiscard]] bool value() const;

    [[nodiscard]] Kind kind() const override;

    [[nodiscard]] std::string inspect() const override;

private:
    bool _value;
};

class Null : public Object {
public:
    [[nodiscard]] Kind kind() const override;

    [[nodiscard]] std::string inspect() const override;
};

}


#endif //MATCHBOXDB_OBJECT_H
