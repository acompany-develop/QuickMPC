#pragma once
#include <iomanip>
#include <iostream>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <vector>
namespace qmpc {

class Group;
class BigInt {
  BIGNUM *num;

public:
  BigInt() { num = BN_new(); }
  BigInt(const BigInt &other) { num = other.num; }
  BigInt(BIGNUM *num) : num(num) {}
  ~BigInt() {
    if (num != nullptr)
      BN_free(num);
  }
  unsigned char *toBinary() {
    unsigned char *ret;
    BN_bn2bin(num, ret);
    return ret;
  }
  static BigInt fromBinary(const unsigned char *input, size_t length) {
    BigInt ret(BN_bin2bn(input, length, nullptr));
    return ret;
  }
  BIGNUM *getValue() { return num; }

  const BIGNUM *getValue() const { return num; }
};
class Point {
public:
  friend class Group;
  EC_POINT *value;
  Group *group;

  Point(Group *group);
  ~Point();
  Point(const Point &other) {
    group = other.group;
    value = other.value;
  }
  size_t size();
  std::vector<unsigned char> to_bin();
  Point add(const Point &);
  Point mul(const BigInt &);
  Point inv();
  bool operator==(const Point &);
};
class Group {
  static constexpr size_t size = 256;
  EC_GROUP *ec_group = nullptr;
  BN_CTX *bn_ctx = nullptr;
  BigInt order;
  unsigned char *scratch;

public:
  Group() : order() {
    ec_group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
    bn_ctx = BN_CTX_new();
    EC_GROUP_get_order(ec_group, order.getValue(), bn_ctx);
    scratch = new unsigned char[256];

    char *number_str = BN_bn2hex(order.getValue());

    std::cout << "order is " << number_str << std::endl;

    Point point{this};
    Point p = get_generator();
  }
  ~Group() {
    if (ec_group != nullptr)
      EC_GROUP_free(ec_group);
    if (bn_ctx != nullptr)
      BN_CTX_free(bn_ctx);
    if (scratch != nullptr)
      delete[] scratch;
  }
  inline auto getPointer() { return ec_group; }
  inline auto getCtx() { return bn_ctx; }
  inline Point get_generator() {
    Point res(this);
    int ret = EC_POINT_copy(res.value, EC_GROUP_get0_generator(ec_group));

    BIGNUM *x = BN_new();
    BIGNUM *y = BN_new();

    if (EC_POINT_get_affine_coordinates_GFp(ec_group, res.value, x, y, NULL)) {
      BN_print_fp(stdout, x);
      putc('\n', stdout);
      BN_print_fp(stdout, y);
      putc('\n', stdout);
    }
    std::cout << "point size " << res.size() << std::endl;
    auto poB = res.to_bin();
    for (auto &a : poB) {
      std::cout << std::setfill('0') << std::setw(2) << std::hex
                << static_cast<unsigned int>(a) << std::endl;
    }
    return res;
  }
};
} // namespace qmpc