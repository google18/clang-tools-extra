// RUN: %check_clang_tidy %s abseil-anonymous-enclosed-aliases %t
namespace bar {

class something {

};
}

namespace foo {

using bar::something;

namespace {

}  // anonymous namespace
}  // namespace foo
// CHECK-MESSAGES: :[[@LINE-6]]:12: warning: UsingDecl 'something' should be in the anonymous namespace. See: https://abseil.io/tips/119 [abseil-anonymous-enclosed-aliases]


namespace foo {

namespace {

using ::bar::something;

}  // anonymous namespace
}  // namespace foo
