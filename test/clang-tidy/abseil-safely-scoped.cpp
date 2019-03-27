// RUN: %check_clang_tidy %s abseil-safely-scoped %t
namespace bar {

class something {};
} // namespace bar

namespace foo {

using bar::something;

namespace {} // anonymous namespace
} // namespace foo
// CHECK-MESSAGES: :[[@LINE-4]]:12: warning: using declaration 'something' not
// declared in the innermost namespace. [abseil-safely-scoped]

namespace foo {

namespace {

using ::bar::something;

} // anonymous namespace
} // namespace foo

// Check should not be triggered below when we are at 
// function (instead of namespace) scope.
namespace outer {

  int fun_scope() {
    using ::bar::something;
    return 0;
  } // function scope
  
  namespace inner {

  } // namespace inner

} // namespace outer

