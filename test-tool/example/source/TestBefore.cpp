#include <memory>

int main() {
  std::unique_ptr<int> ptr(new int(42));
}
