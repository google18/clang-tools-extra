// RUN: %check_clang_tidy %s abseil-safely-scoped %t
namespace bar {

class A {};
class B {};

} // namespace bar

namespace foo1 {

// CHECK-MESSAGES: :[[@LINE+4]]:12: warning: using declaration 'A' is not
// declared in the innermost namespace. Use discretion when moving using
// declarations as it might necessitate moving lines containing relevant
// aliases. [abseil-safely-scoped]
using bar::A;
void f(A a);

namespace {} // anonymous namespace

} // namespace foo1

namespace foo2 {

namespace {

using ::bar::B;

} // anonymous namespace

void g(B b);

} // namespace foo2

// Check should not be triggered below when we are at
// function (instead of namespace) scope.
namespace outer {

int fun_scope() {
  using ::bar::A;
  return 0;
} // function scope

class Base {
public:
  void f();
}; // class scope

class Derived : public Base {
public:
  using Base::f;
}; // class scope

namespace inner {} // namespace inner

} // namespace outer
