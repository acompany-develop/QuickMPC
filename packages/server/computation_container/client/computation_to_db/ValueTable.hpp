#pragma once

#include <string>
#include <vector>

#include "share/share.hpp"

namespace qmpc::ComputationToDb
{
class ValueTable
{
    using Share = ::Share;
    const std::vector<std::vector<std::string>> table;
    const std::vector<std::string> schemas;

    std::vector<Share> getColumn(int) const;

public:
    ValueTable(const std::vector<std::vector<std::string>> &, const std::vector<std::string> &);
    bool operator==(const ValueTable &) const;

    std::vector<std::vector<std::string>> getTable() const;
    std::vector<std::string> getSchemas() const;
    ValueTable vjoin(const ValueTable &, int, int) const;
    ValueTable hjoin(const ValueTable &, int, int) const;
    ValueTable hjoinShare(const ValueTable &, int, int) const;
};

ValueTable
parseRead(const std::vector<ValueTable> &, const std::vector<int> &, const std::vector<int> &);
}  // namespace qmpc::ComputationToDb
