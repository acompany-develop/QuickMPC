#pragma once

#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/operators.hpp>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include "external/proto/computation_to_computation_container/computation_to_computation.grpc.pb.h"

namespace qmpc::Utils
{
using mp_int = boost::multiprecision::cpp_int;
using mp_float = boost::multiprecision::number<boost::multiprecision::cpp_dec_float<50>>;
using BIB = computationtocomputation::BigIntByte;

class FixedPoint : private boost::operators<FixedPoint>
{
private:
    constexpr static int shift = 100'000'000;
    mp_int value;
public:
    FixedPoint() : value(0) {}
    FixedPoint(const FixedPoint &v) : value(v.value) {}
    FixedPoint(FixedPoint &&v) : value(std::move(v.value)) {}
    template <typename T, std::enable_if_t<
        std::is_arithmetic_v<std::remove_reference_t<T>>,
        std::nullptr_t> = nullptr>
    FixedPoint(const T &v)
    {
        mp_float tmp = static_cast<mp_float>(v);
        tmp *= shift;
        value = static_cast<mp_int>(tmp);
    }
    FixedPoint(const std::string &str)
    {
        mp_float v_{str};
        value = static_cast<mp_int>(v_ * shift);
    }
    FixedPoint(const BIB &P)
    {
        std::string bytes = P.byte();
        import_bits(value, bytes.begin(), bytes.end(), 8);
        if(P.sgn())
        {
            value *= -1;
        }
    }

    // shift をかけずに代入する i.e., raw(1) は実質的に 10^-8 を代入することに対応
    template <typename T>
    static FixedPoint raw(const T &v)
    {
        FixedPoint ret{};
        ret.value = v;
        return ret;
    }

    FixedPoint &operator=(const FixedPoint &obj)
    {
        value = obj.value;
        return *this;
    }
    FixedPoint &operator=(FixedPoint &&obj)
    {
        value = std::move(obj.value);
        return *this;
    }

    template <typename T = mp_int>
    T getShiftedVal() const
    {
        return value.template convert_to<T>();
    }
    double getDoubleVal() const
    {
        mp_float ret = static_cast<mp_float>(value);
        return static_cast<double>(ret / shift);
    }
    mp_int getRoundValue() const
    {
        mp_int ret = value / shift;
        if ((value % shift) * 2 >= shift)
        {
            ret++;
        }
        return ret;
    }
    std::string getStrVal() const
    {
        mp_float ret = static_cast<mp_float>(value);
        ret /= shift;
        return ret.str(20, std::ios_base::fixed);
    }
    BIB getBytes() const
    {
        BIB ret;
        ret.set_sgn(value < 0);
        std::string bytes;
        export_bits(value, std::back_inserter(bytes), 8);
        ret.set_byte(bytes);
        return ret;
    }
    void setShare(computationtocomputation::Share& share) const
    {
        auto fp = share->mutable_fp();
        fp = (*this).getBytes();
    }

    FixedPoint operator-() const
    {
        FixedPoint ret{};
        ret.value = -value;
        return ret;
    }

    FixedPoint &operator+=(const FixedPoint &obj)
    {
        value += obj.value;
        return *this;
    }
    FixedPoint &operator-=(const FixedPoint &obj)
    {
        value -= obj.value;
        return *this;
    }
    FixedPoint &operator*=(const FixedPoint &obj)
    {
        value *= obj.value;
        value /= shift;
        return *this;
    }
    FixedPoint &operator/=(const FixedPoint &obj)
    {
        value *= shift;
        value /= obj.value;
        return *this;
    }
    FixedPoint &operator%=(const FixedPoint &obj)
    {
        value %= obj.value;
        return *this;
    }
    FixedPoint &operator++()
    {
        value += shift;  // shift分ずれるので1*shift
        return *this;
    }
    FixedPoint &operator--()
    {
        value -= shift;  // shift分ずれるので1*shift
        return *this;
    }
    bool operator==(const FixedPoint &obj) const noexcept
    {
        return (value - obj.value >= -1 and value - obj.value <= 1) ? true : false;
    }
    bool operator<(const FixedPoint &obj) const noexcept { return value < obj.value; }

    friend std::ostream &operator<<(std::ostream &os, const FixedPoint &fp)
    {
        os << std::fixed;
        os << std::setprecision(10);
        os << fp.value.template convert_to<mp_float>() / shift;
        os << std::resetiosflags(std::ios_base::floatfield);
        return os;
    }
};
}  // namespace qmpc::Utils

using FixedPoint = qmpc::Utils::FixedPoint;
