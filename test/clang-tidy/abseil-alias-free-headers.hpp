// RUN: %check_clang_tidy %s abseil-alias-free-headers %t
  
namespace foo {
  void f();
}

using foo::f;
// CHECK-MESSAGES: :[[@LINE-1]]:12: warning: convenience aliases in header files are dangerous [abseil-alias-free-headers]

void other_function();
