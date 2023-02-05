#include "client.hpp"

#include <grpc/grpc.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

namespace qmpc::ComputationToBts
{
Client::Client() noexcept
{
    auto dest = Config::getInstance()->cc_to_bts;
    stub_ = createStub<enginetobts::EngineToBts>(dest);
    QMPC_LOG_INFO("{:<15} Client {:<30} is Active", "[Cc2Bts]", dest.url);
}

std::shared_ptr<Client> Client::getInstance()
{
    static auto instance = std::make_shared<Client>();
    return instance;
}

}  // namespace qmpc::ComputationToBts
