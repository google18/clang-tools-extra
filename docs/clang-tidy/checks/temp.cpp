class A {
public:
  static A *NewA() { return new A(); }

private:
  A() {}
};

std::unique_ptr<A> a;

// Original - reset called with a static function returning a std::unqiue_ptr
a.reset(A::NewA());

// Suggested - reset ptr with absl::WrapUnique
a = absl::WrapUnique(A::NewA());

// Original - std::unique_ptr initialized with static function
std::unique_ptr<A> b(A::NewA());

// Suggested - initialize with absl::WrapUnique instead
auto b = absl::WrapUnique(A::NewA())
