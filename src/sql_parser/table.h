#ifndef MATCHBOXDB_TABLE_H
#define MATCHBOXDB_TABLE_H

#include "node.h"

namespace ast {

class Table : public Node {
public:
    explicit Table(std::string table_name);

    void repr(std::ostream &os) const override;

    std::string table_name() const;

protected:
    std::string _table_name;
};

}

#endif //MATCHBOXDB_TABLE_H
