#include <sodium.h>
#include <boost/asio.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <iostream>
#include <thread>
#include <type_traits.hpp>
#include "Socket.hpp"

namespace qmpc
{
class OT
{
public:
    OT() = default;
    template <typename T>
    void send(int to_id, const std::vector<T>& messages)
    {
    }
    template <typename T>
    T recieve()
    {
        T ret;
    }
};
}  // namespace qmpc