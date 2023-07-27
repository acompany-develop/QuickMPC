#pragma once
#include "external/proto/engine_to_bts/engine_to_bts.grpc.pb.h"

#include <string>
#include <vector>

#include "fixed_point/fixed_point.hpp"

namespace qmpc::BtsHandler::BTSJobType
{
namespace etb = enginetobts;
struct Triple
{
    using result_type = std::tuple<FixedPoint, FixedPoint, FixedPoint>;
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
            ret[i] = std::make_tuple(toFP(triple.a()), toFP(triple.b()), toFP(triple.c()));
        }
        return ret;
    }

private:
    FixedPoint toFP(const etb::BigIntByte& a)
    {
        bool sgn = a.sgn();
        std::string abs_byte = a.byte();
        return FixedPoint(std::make_pair(sgn, abs_byte));
    }
};

struct RandBit
{
    using result_type = std::int64_t;
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
}  // namespace qmpc::BtsHandler::BTSJobType