#include "Client.hpp"

#include <unistd.h>

#include <chrono>
#include <string>
#include <thread>

#include "LogHeader/Logger.hpp"
#include "Share/AddressId.hpp"

namespace qmpc::ComputationToComputation
{

Client::Client(const Url &endpoint) noexcept
{
    stub_ = createStub<computationtocomputation::ComputationToComputation>(endpoint);
    QMPC_LOG_INFO("{:<15} Client {:<30} is Active", "[Cc2Cc]", endpoint.url);
}

std::shared_ptr<Client> Client::getPtr(const Url &endpoint)
{
    /*
    shared_ptrでprivate constructorを管理するために作成したクラス
    外部からは作成できないため一時的にpublic constructorを作成できる
    スコープを抜けると消滅するため安全
    返却する型は元のクラスになるのでupCastされてオブジェクトはClientになる
    */
    struct impl : Client
    {
        impl(const Url &url) : Client(url) {}
    };
    auto ptr = std::make_shared<impl>(endpoint);
    return std::move(ptr);
}

}  // namespace qmpc::ComputationToComputation
