// RUN: %check_clang_tidy %s abseil-anonymous-enclosed-aliases %t
namespace bar {

class A {};
class B {};

} // namespace bar

namespace foo1 {

// CHECK-MESSAGES: :[[@LINE+4]]:12: warning: using declaration 'A' should
// be in the anonymous namespace. Use discretion when moving using declarations
// as it might necessitate moving lines containing relevant aliases.
// [abseil-anonymous-enclosed-aliases]
using bar::A;
void f(A a);

namespace {} // anonymous namespace

} // namespace foo1

namespace foo2 {

namespace {

// This is okay
using ::bar::B;

} // anonymous namespace

void g(B b);

} // namespace foo2

// Check should not be triggered below when the using declaration is at
// function or class (instead of namespace) scope.
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

namespace {} // anonymous namespace

} // namespace outer