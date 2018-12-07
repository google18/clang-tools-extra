// RUN: %check_clang_tidy %s abseil-qualified-aliases %t

namespace foo {
  void f();
  void correct();
}

namespace bar {
  using foo::f;
// CHECK-MESSAGES: :[[@LINE-1]]:9: warning: using declaration is not fully qualified: see https://abseil.io/tips/119 [abseil-qualified-aliases]
  using ::foo::correct;
}

namespace outermost {
  namespace middle {
    namespace innermost {

      enum Color {Red, Blue, Yellow};

    } // namespace innermost

    using innermost::Color;
// CHECK-MESSAGES: :[[@LINE-1]]:11: warning: using declaration is not fully qualified: see https://abseil.io/tips/119 [abseil-qualified-aliases]

  } // namespace middle
} // namespace example
