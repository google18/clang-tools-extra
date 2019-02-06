#include <memory>
#include "absl/memory/memory.h"

int main() {
  std::unique_ptr<int> upi = absl::make_unique<int>(42);
}
