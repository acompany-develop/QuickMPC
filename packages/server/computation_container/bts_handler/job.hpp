#pragma once
#include "external/proto/engine_to_bts/engine_to_bts.grpc.pb.h"

#include <string>
#include <vector>

namespace qmpc::BtsHandler::JobType
{
namespace etb = enginetobts;

template <typename T>
struct Triple
{
    using value_type = T;
    using result_type = std::tuple<T, T, T>;
    using response_type = etb::GetTriplesResponse;
    static inline std::string op_name = "GetTriples";

    static grpc::Status request(
        const std::unique_ptr<etb::EngineToBts::Stub>& stub,
        grpc::ClientContext& context,
        etb::GetRequest request,
        response_type& response
    )
    {
        return stub->GetTriples(&context, request, &response);
    }

    static std::vector<result_type> getValue(const etb::GetTriplesResponse& response)
    {
        size_t length = response.triples_size();
        std::vector<result_type> ret(length);

        for (size_t i = 0; i < length; i++)
        {
            auto triple = response.triples(i);
            ret[i] = std::make_tuple(T(triple.a()), T(triple.b()), T(triple.c()));
        }
        return ret;
    }
};

template <typename T>
struct RandBit
{
    using value_type = T;
    using result_type = T;
    using response_type = etb::GetRandBitsResponse;
    static inline std::string op_name = "GetRandBits";

    static grpc::Status request(
        const std::unique_ptr<etb::EngineToBts::Stub>& stub,
        grpc::ClientContext& context,
        etb::GetRequest request,
        response_type& response
    )
    {
        return stub->GetRandBits(&context, request, &response);
    }

    static std::vector<result_type> getValue(const etb::GetRandBitsResponse& response)
    {
        size_t length = response.randbits_size();
        std::vector<result_type> ret(length);

        for (size_t i = 0; i < length; i++)
        {
            ret[i] = response.randbits(i);
        }
        return ret;
    }
};
}  // namespace qmpc::BtsHandler::JobType