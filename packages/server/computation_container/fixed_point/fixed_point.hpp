#pragma once

#include <boost/math/special_functions/fpclassify.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/operators.hpp>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <type_traits>
#include "external/proto/common_types/common_types.grpc.pb.h"

namespace qmpc::Utils
{
using mp_int = boost::multiprecision::cpp_int;
using mp_float = cpp_dec_float_50;  // 50 > 19*2 + 8
using pbBigIntByte = pb_commontypes::BigIntByte;

class FixedPoint : private boost::operators<FixedPoint>
{
private:
    constexpr static long long shift = 100'000'000;
    mp_int value;

public:
    FixedPoint() : value(0) {}
    FixedPoint(const FixedPoint &v) : value(v.value) {}
    FixedPoint(FixedPoint &&v) : value(std::move(v.value)) {}
    template <
        typename T,
        std::enable_if_t<std::is_arithmetic_v<std::remove_reference_t<T>>, std::nullptr_t> =
            nullptr>
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
    FixedPoint(const pbBigIntByte &P)
    {
        std::string abs_byte = P.abs_byte();
        import_bits(value, abs_byte.begin(), abs_byte.end(), 8);
        if (P.is_minus())
        {
            value *= -1;
        }
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
    T getVal() const
    {
        // getVal() より getShiftedVal() の方が適切かもしれない
        return value.template convert_to<T>();
    }
    std::string getStrVal() const
    {
        mp_float ret = static_cast<mp_float>(value);
        ret /= shift;
        std::string s = ret.str(0, std::ios_base::fixed);
        while (s.size() and s.back() == '0')
        {
            s.pop_back();
        }
        return s;
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
    pbBigIntByte getBigIntByte() const
    {
        bool is_minus = value < 0;
        std::string bytes;
        export_bits(value, std::back_inserter(bytes), 8);
        pbBigIntByte ret;
        ret.set_is_minus(is_minus);
        ret.set_abs_byte(byte);
        return ret;
    }
    template <typename T = double>
    T getDoubleVal() const
    {
        mp_float ret = static_cast<mp_float>(value) / shift;
        if constexpr (std::is_same_v<T, mp_float>)
        {
            return ret;
        }
        else
        {
            return static_cast<T>(ret);
        }
    }
    constexpr static auto getShift() noexcept { return shift; }

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
        value /= shift;  // 整数で保持するため余分なshiftを除算する
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
        /*
        todo : value == obj.value に修正
        test で EXPECT_EQ(a,b) を用いている箇所は
            EXPECT_NEAR(a.getDoubleVal(), b.getDoubleVal(), 1e-7) のように明示的に書く
        ただし加減算だけなど, 誤差が出ないことが仕様として保証されている場合は EXPECT_EQ
        のままで良い
        */
        return abs(value - obj.value) <= 10;
    }
    bool operator<(const FixedPoint &obj) const noexcept { return value < obj.value; }
    /*
    標準入出力に入力するとShiftで割った結果が出力される。
    FixedPoint a{5};
    std::cout << a << std::endl;

    5が出力される

    */
    friend std::ostream &operator<<(std::ostream &os, const FixedPoint &fp)
    {
        os << std::fixed;
        os << std::setprecision(8);
        os << fp.value.template convert_to<mp_float>() / shift;
        os << std::resetiosflags(std::ios_base::floatfield);
        return os;
    }
};
inline auto abs(const FixedPoint &x)
{
    if (x < FixedPoint("0.0"))
    {
        return -x;
    }
    else
    {
        return x;
    }
}
template <typename FP>
std::string to_string(const FP &fp)
{
    static_assert(std::is_same_v<FP, FixedPoint>, "for FP");
    return fp.getStrVal();
}
}  // namespace qmpc::Utils
using FixedPoint = qmpc::Utils::FixedPoint;
