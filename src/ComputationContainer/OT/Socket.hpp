
namespace qmpc
{

class Socket
{
    asio::io_service io_service;

    tcp::acceptor acc(io_service, tcp::endpoint(tcp::v4(), 31400));
    tcp::socket socket(io_service);

    tcp::resolver resolver(io_service);

public:
};
}  // namespace qmpc