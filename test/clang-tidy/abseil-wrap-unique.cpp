// RUN: %check_clang_tidy %s abseil-wrap-unique %t

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
 public:
  static A* NewA() {
    return new A();
  }

 private:
  A() {}
};

class B {
 public:
  static B* NewB(int bIn) {
    return new B();
  }

 private:
  B() {}
};

struct C {
  int x;
  int y;
};

`void positives() {
  std::unique_ptr<A> a;
  a.reset(A::NewA());
  //CHECK-MESSAGE: :[[@LINE-1]]:3: warning: prefer absl::WrapUnique for resetting unique_ptr [abseil-wrap-unique]
  //CHECK-FIXES: a = absl::WrapUnique(A::NewA())
  
  std::unique_ptr<A> b(A::NewA());
  //CHECK-MESSAGE: :[[@LINE-1]]:3: warning: Perfer absl::WrapUnique to constructing unique_ptr [abseil-wrap-unique]
  //CHECK-FIXES: auto b = absl::WrapUnique(A::NewA())

  int cIn;
  std::unique_ptr<B> c(B::NewB(cIn));
  //CHECK-MESSAGE: :[[@LINE-1]]:3: warning: Perfer absl::WrapUnique to constructing unique_ptr [abseil-wrap-unique]
  //CHECK-FIXES: auto c = absl::WrapUnique(B::NewB(cIn))

  int dIn;
  std::unique_ptr<B> d;
  d.reset(B::NewB(dIn));
  //CHECK-MESSAGE: :[[@LINE-1]]:3: warning: prefer absl::WrapUnique for resetting unique_ptr [abseil-wrap-unique]
  //CHECK-FIXES: d = absl::WrapUnique(B::NewB(dIn))
  
  auto e = std::unique_ptr<A>(A::NewA());
  //CHECK-MESSAGE: :[[@LINE-1]]:3: warning: prefer absl::WrapUnique for resetting unique_ptr [abseil-wrap-unique]
  //CHECK-FIXES: e = absl::WrapUnique(A::NewA())

  //std::unique_ptr<int> e(new int[2] {1,2});
}

