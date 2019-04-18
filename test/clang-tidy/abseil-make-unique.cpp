// RUN: %check_clang_tidy %s abseil-make-unique %t -- -- -std=c++11 \
// RUN:   -I%S/Inputs/modernize-smart-ptr

#include "unique_ptr.h"
#include "initializer_list.h"
// CHECK-FIXES: #include "absl/memory/memory.h"

class A {
 int x;
 int y;

 public:
   A(int _x, int _y): x(_x), y(_y) {}
};

struct B {
  B(std::initializer_list<int>);
  B();
};

struct Base {
  Base();
};

struct Derived : public Base {
  Derived();
};

int* returnPointer();

std::unique_ptr<int> makeAndReturnPointer() {
  return std::unique_ptr<int>(new int(0));
  // CHECK-MESSAGES: :[[@LINE-1]]:10: warning: use absl::make_unique instead [abseil-make-unique]
  // CHECK-FIXES: return absl::make_unique<int>(0);
}

void Positives() {
  std::unique_ptr<int> P1 = std::unique_ptr<int>(new int(1));
  // CHECK-MESSAGES: :[[@LINE-1]]:29: warning: use absl::make_unique instead [abseil-make-unique]
  // CHECK-FIXES: std::unique_ptr<int> P1 = absl::make_unique<int>(1);

  P1.reset(new int(2));
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: use absl::make_unique instead [abseil-make-unique]
  // CHECK-FIXES: P1 = absl::make_unique<int>(2);

  // Non-primitive paramter
  std::unique_ptr<A> P2 = std::unique_ptr<A>(new A(1, 2));
  // CHECK-MESSAGES: :[[@LINE-1]]:27: warning: use absl::make_unique instead [abseil-make-unique]
  // CHECK-FIXES: std::unique_ptr<A> P2 = absl::make_unique<A>(1, 2);

  P2.reset(new A(3, 4));
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: use absl::make_unique instead [abseil-make-unique]
  // CHECK-FIXES: P2 = absl::make_unique<A>(3, 4);
}

void Negatives() {
  // Only warn if explicitly allocating a new object
  std::unique_ptr<int> R = std::unique_ptr<int>(returnPointer());
  R.reset(returnPointer());

  // Only replace if the template type is same as new type
  auto Pderived = std::unique_ptr<Base>(new Derived());

  // Ignore initializer-list constructors
  std::unique_ptr<B> PInit = std::unique_ptr<B>(new B{1, 2});
  PInit.reset(new B{1, 2});
}
