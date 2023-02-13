
#include "value_table_new.hpp"

#include "client/computation_to_db/client.hpp"
#include "logging/logger.hpp"

namespace qmpc::ComputationToDb
{

// nullでない場合にコンテナのbegin()を返す
std::optional<TableType::iterator> begin(std::optional<TableType> &row)
{
    if (!row)
    {
        return std::nullopt;
    }
    return row.value().begin();
}

TableIterator::TableIterator(const std::string &data_id)
    : data_id(data_id)
    , piece_id(0)
    , piece_table(Client::getInstance()->readTable(data_id, piece_id))
    , itr(begin(piece_table))
{
}

RowType TableIterator::operator*()
{
    if (!itr)
    {
        // TODO: QMPCのerror logを使う
        throw std::runtime_error("");
    }
    return *(itr.value());
}
TableIterator &TableIterator::operator++()
{
    if (!itr || !piece_table)
    {
        // TODO: QMPCのerror logを使う
        throw std::runtime_error("");
    }

    ++itr.value();
    if (itr.value() == piece_table.value().end())
    {
        ++piece_id;
        piece_table = Client::getInstance()->readTable(data_id, piece_id);
        itr = begin(piece_table);
    }
    return *this;
}
bool TableIterator::operator!=(const TableIterator &tgt) { return itr != tgt.itr; }

ValueTable::ValueTable(const std::string &data_id) : data_id(data_id) {}
TableIterator ValueTable::begin() const { return TableIterator(data_id); }
TableIterator ValueTable::end() const { return TableIterator(std::nullopt); }

TableType ValueTable::getTable() const
{
    TableType table;
    for (const auto &row : *this)
    {
        table.emplace_back(row);
    }
    return table;
}
std::vector<std::string> ValueTable::getSchemas() const
{
    return Client::getInstance()->readSchema(data_id);
}
}  // namespace qmpc::ComputationToDb
