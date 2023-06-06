#pragma once

#include <optional>
#include <string>
#include <vector>

#include "client/computation_to_db/client.hpp"

namespace qmpc::ComputationToDb
{
using RowType = std::vector<std::string>;
using TableType = std::vector<RowType>;

class TableIterator
{
    const std::string data_id;
    int piece_id;
    std::optional<TableType> piece_table;
    std::optional<TableType::iterator> itr;

public:
    TableIterator(const std::string &data_id);
    TableIterator(const std::optional<TableType::iterator> &itr) : itr(itr) {}

    RowType operator*();
    TableIterator &operator++();
    bool operator!=(const TableIterator &tgt);
    bool operator==(const TableIterator &tgt);
};

class ValueTable
{
    const std::string data_id;

public:
    ValueTable(const std::string &);
    TableIterator begin() const;
    TableIterator end() const;

    std::string getDataId() const;
    int getMatchinColumnNumber() const;
    TableType getTable() const;
    std::vector<std::string> getColumn(int) const;
    std::vector<std::string> getIdColumn() const;
    std::vector<SchemaType> getSchemas() const;
};

}  // namespace qmpc::ComputationToDb
