#include "Group.hpp"

namespace qmpc {

Point::Point(Group *g) {
  std::cout << "Point constructor " << std::endl;
  if (g == nullptr)
    return;
  this->group = g;
  value = EC_POINT_new(g->getPointer());
}
Point::~Point() {
  if (value != nullptr)
    EC_POINT_free(value);
}

size_t Point::size() {
  size_t ret = EC_POINT_point2oct(group->getPointer(), value,
                                  POINT_CONVERSION_UNCOMPRESSED, NULL, 0,
                                  group->getCtx());
  return ret;
}

std::vector<unsigned char> Point::to_bin() {
  std::vector<unsigned char> ret(this->size());
  int status = EC_POINT_point2oct(group->getPointer(), value,
                                  POINT_CONVERSION_UNCOMPRESSED, ret.data(),
                                  this->size(), group->getCtx());

  return ret;
}

Point Point::add(const Point &rhs) {
  Point ret(group);
  int res = EC_POINT_add(group->getPointer(), ret.value, value, rhs.value,
                         group->getCtx());
  return ret;
}

Point Point::mul(const BigInt &m) {
  Point ret(group);
  int res = EC_POINT_mul(group->getPointer(), ret.value, NULL, value,
                         m.getValue(), group->getCtx());
  return ret;
}

Point Point::inv() {
  Point ret(*this);
  int res = EC_POINT_invert(group->getPointer(), ret.value, group->getCtx());
  return ret;
}
bool Point::operator==(const Point &rhs) {
  int ret =
      EC_POINT_cmp(group->getPointer(), value, rhs.value, group->getCtx());
  return (ret == 0);
}
} // namespace qmpc