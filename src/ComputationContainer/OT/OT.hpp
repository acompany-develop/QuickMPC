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
    struct sender_tag
    {
    };
    struct reciever_tag
    {
    };
    std::vector<Socket> socket;

public:
    OT() = default;
};
}  // namespace qmpc