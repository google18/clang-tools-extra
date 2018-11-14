// RUN: %check_clang_tidy %s abseil-make-unique %t
namespace std {
template <typename T>
struct default_delete {};

template <typename T, typename D = default_delete<T>>
class unique_ptr {
 public:
  unique_ptr();
  ~unique_ptr();
  explicit unique_ptr(T*);
  template <typename U, typename E>
  unique_ptr(unique_ptr<U, E>&&);
  T* release();
  void reset(T*);
};
}  // namespace std


void Positives() {
  std::unique_ptr<int> upi(new int(1));
  //
  std::unique_ptr<int> toReset;
  toReset.reset(new int(2));
  //
}

void Negatives(){
  std::unique_ptr<int> upi;
}
