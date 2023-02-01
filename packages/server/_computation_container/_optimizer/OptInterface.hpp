#pragma once

#include "ObjectiveFunction/ObjectiveFunctionInterface.hpp"

namespace qmpc::Optimizer
{
class OptInterface
{
    using Share = ::Share;
    using interface = qmpc::ObjectiveFunction::ObjectiveFunctionInterface;

public:
    virtual std::vector<Share> optimize(
        int iterationNum, const interface &f, const std::vector<Share> &theta
    ) const = 0;
};
}  // namespace qmpc::Optimizer