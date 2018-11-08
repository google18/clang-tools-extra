// RUN: %check_clang_tidy %s abseil-qualified-aliases %t

namespace foo {
  void f();
  void correct();
}

namespace bar {
  using foo::f;
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: using declaration is not fully qualified [abseil-qualified-aliases]
  using ::foo::correct;
}

//   * Make the CHECK patterns specific enough and try to make verified lines
//     unique to avoid incorrect matches.
//   * Use {{}} for regular expressions.
// CHECK-FIXES: {{^}}  using ::foo::f;{{$}}

