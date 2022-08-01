#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/operators.hpp>
#include <type_traits>
#include <exception>
#include "Logging/Logger.hpp"
namespace qmpc
{
template <typename T>
T modpow(T a, T b);
template <typename T = boost::multiprecision::checked_uint128_t>
class PrimeField : private boost::operators<PrimeField<T>>
{
private:
    T value;  // 値

public:
    using type = T;
    // TODO:これらの定数にそれぞれsigned int256対応が必要
    // 詳細は Docs/extending-the-fixed-point-range.md
    inline static const T p = 18446744073709551557ull;              // 素体の要素数(2^64 - 59)
    static constexpr int l = 64;                                    // pのビット長
    static constexpr int lambda = 8;                                // lの平方根
    inline static const T inverse_of_two = 9223372036854775779ull;  // mod p における 2の逆元

    PrimeField() : value(0){};
    ~PrimeField(){};
    PrimeField(const PrimeField &p) : value(p.value){};
    template <
        typename U,
        std::enable_if_t<
            std::is_integral_v<
                std::remove_reference_t<U>> or std::is_convertible_v<std::remove_reference_t<U>, T>,
            std::nullptr_t> = nullptr>
    PrimeField(const U &obj)
    {
        if (obj < 0 or obj > PrimeField::p)
        {
            qmpc::Log::throw_with_trace(
                std::runtime_error("PrimeField: 変換不可能な値が入力されています")
            );
        }
        value = obj;
        value %= PrimeField::p;
    }
    PrimeField(const std::string &str) : PrimeField(T{str}) {}
    T getVal() const noexcept { return value; }
    std::string getStrVal() const { return value.str(); }
    PrimeField getInv() const { return modpow(this->value, PrimeField::p - 2); }
    PrimeField operator-() const
    {
        PrimeField ret{*this};
        ret.value = PrimeField::p - ret.value;
        return ret;
    }

    PrimeField &operator+=(const PrimeField &obj)
    {
        value += obj.value;
        value %= PrimeField::p;
        return *this;
    }
    PrimeField &operator-=(const PrimeField &obj)
    {
        value += PrimeField::p - obj.value;
        value %= PrimeField::p;
        return *this;
    }
    PrimeField &operator*=(const PrimeField &obj)
    {
        value *= obj.value;
        value %= PrimeField::p;
        return *this;
    }
    PrimeField &operator/=(const PrimeField &obj)
    {
        auto tmp = obj.getInv();
        value *= tmp.value;
        value %= PrimeField::p;
        return *this;
    }
    PrimeField &operator%=(const PrimeField &obj)
    {
        value %= obj.value;
        return *this;
    }
    PrimeField &operator%=(const T &obj)
    {
        value %= obj;
        return *this;
    }
    constexpr bool operator==(const PrimeField &obj) const { return this->value == obj.value; }
    constexpr bool operator<(const PrimeField &obj) const { return this->value < obj.value; }
};
// a^b mod p
// verified
template <typename T = boost::multiprecision::checked_uint128_t>
T modpow(T a, T b)
{
    T ret = 1;
    while (b)
    {
        if (b & 1) ret = ret * a % PrimeField<T>::p;
        a = a * a % PrimeField<T>::p;
        b >>= 1;
    }
    return ret;
}
// aがpの平方剰余であるか返す
// verified
template <typename T = boost::multiprecision::checked_uint128_t>
bool Legendre(T a)
{
    T half_p = PrimeField<T>::p / 2;
    T ret = modpow(a, half_p);
    if (ret == PrimeField<T>::p - 1) return false;  // 平方剰余でない

    return true;  // 平方剰余である
}
// return sqrt(x) mod p
// more information, see http://techtipshoge.blogspot.com/2015/04/blog-post_5.html
// verified
template <typename T = boost::multiprecision::checked_uint128_t>
PrimeField<T> modsqrt(PrimeField<T> x)
{
    // step 1
    T n = x.getVal();
    // p-1 = Q*2^S
    const T Q = 4611686018427387889ull;
    const T S = 2;

    // step 2
    // const T z = 2;                // 法pにおいて，最初に出現する非平方剰余数
    T c = 2296021864060584341ull;  // c = modpow(z, Q);

    // step 3
    T R = modpow(n, (Q + 1) / 2);
    T t = modpow(n, Q);
    T M = S;

    // step 4
    while (t != 1)
    {
        T i;
        T t2 = t;
        for (i = 1; i < M; i++)
        {
            t2 = t2 * t2 % PrimeField<T>::p;
            if (t2 == 1) break;
        }

        T b = modpow(c, modpow(T(1), M - i - 1));
        R = R * b % PrimeField<T>::p;
        t = (t * b % PrimeField<T>::p) * b % PrimeField<T>::p;
        c = b * b % PrimeField<T>::p;
        M = i;
    }

    if (R <= PrimeField<T>::p / 2)
    {
        return PrimeField(R);
    }
    else
    {
        return PrimeField(PrimeField<T>::p - R);
    }
}
}  // namespace qmpc
using PrimeField = qmpc::PrimeField<>;
