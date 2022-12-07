#pragma once

namespace qmpc
{

class OTBase
{
public:
    virtual void send(int to_id, std::vector<int64_t> x) = 0;
    virtual boost::multiprecition::cpp_int recieve(int from_id) = 0;
};
}  // namespace qmpc