// RUN: %check_clang_tidy %s abseil-make-unique %t -- -- -std=c++11 \

// CHECK-FIXES: #include <memory>

namespace std {

template <typename T>
class default_delete {};

template <typename type, typename Deleter = std::default_delete<type>>
class unique_ptr {
public:
  unique_ptr() {}
  unique_ptr(type *ptr) {}
  unique_ptr(const unique_ptr<type> &t) = delete;
  unique_ptr(unique_ptr<type> &&t) {}
  ~unique_ptr() {}
  type &operator*() { return *ptr; }
  type *operator->() { return ptr; }
  type *release() { return ptr; }
  void reset() {}
  void reset(type *pt) {}
  void reset(type pt) {}
  unique_ptr &operator=(unique_ptr &&) { return *this; }
  template <typename T>
  unique_ptr &operator=(unique_ptr<T> &&) { return *this; }

private:
  type *ptr;
};

}  // namespace std

class A {
 int x;
 int y;

 public:
   A(int _x, int _y): x(_x), y(_y) {}
};

struct Base {
  Base();
};

struct Derived : public Base {
  Derived();
};

int* returnPointer();
void expectPointer(std::unique_ptr<int> p);

std::unique_ptr<int> makeAndReturnPointer() {
  // Make smart pointer in return statement
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

  // No arguments to new expression
  std::unique_ptr<int> P3 = std::unique_ptr<int>(new int);
  // CHECK-MESSAGES: :[[@LINE-1]]:29: warning: use absl::make_unique instead [abseil-make-unique]
  // CHECK-FIXES: std::unique_ptr<int> P3 = absl::make_unique<int>();

  P3.reset(new int);
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: use absl::make_unique instead [abseil-make-unique]
  // CHECK-FIXES: P3 = absl::make_unique<int>();

  // Nested parentheses
  std::unique_ptr<int> P4 = std::unique_ptr<int>((new int(3)));
  // CHECK-MESSAGES: :[[@LINE-1]]:29: warning: use absl::make_unique instead [abseil-make-unique]
  // CHECK-FIXES: std::unique_ptr<int> P4 = absl::make_unique<int>(3);

  P4 = std::unique_ptr<int>(((new int(4))));
  // CHECK-MESSAGES: :[[@LINE-1]]:8: warning: use absl::make_unique instead [abseil-make-unique]
  // CHECK-FIXES: P4 = absl::make_unique<int>(4);

  P4.reset((new int(5)));
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: use absl::make_unique instead [abseil-make-unique]
  // CHECK-FIXES: P4 = absl::make_unique<int>(5);

  // With auto
  auto P5 = std::unique_ptr<int>(new int());
  // CHECK-MESSAGES: :[[@LINE-1]]:13: warning: use absl::make_unique instead [abseil-make-unique]
  // CHECK-FIXES: auto P5 = absl::make_unique<int>();

  {
    // No std
    using namespace std;
    unique_ptr<int> Q = unique_ptr<int>(new int());
    // CHECK-MESSAGES: :[[@LINE-1]]:25: warning: use absl::make_unique instead [abseil-make-unique]
    // CHECK-FIXES: unique_ptr<int> Q = absl::make_unique<int>();

    Q = unique_ptr<int>(new int());
    // CHECK-MESSAGES: :[[@LINE-1]]:9: warning: use absl::make_unique instead [abseil-make-unique]
    // CHECK-FIXES: Q = absl::make_unique<int>();
  }

  // Create the unique_ptr as a parameter to a function
  expectPointer(std::unique_ptr<int>(new int()));
  // CHECK-MESSAGES: :[[@LINE-1]]:17: warning: use absl::make_unique instead [abseil-make-unique]
  // CHECK-FIXES: expectPointer(absl::make_unique<int>());
}

void Negatives() {
  // Only warn if explicitly allocating a new object
  std::unique_ptr<int> R = std::unique_ptr<int>(returnPointer());
  R.reset(returnPointer());

  // Only replace if the template type is same as new type
  auto Pderived = std::unique_ptr<Base>(new Derived());
}
