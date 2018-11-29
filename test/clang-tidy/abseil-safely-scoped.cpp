// RUN: %check_clang_tidy %s abseil-safely-scoped %t
namespace bar {

class something {

};
}

namespace foo {

using bar::something;

namespace {

}  // anonymous namespace
}  // namespace foo
// CHECK-MESSAGES: :[[@LINE-6]]:12: warning: UsingDecl 'something' should be in the innermost scope [abseil-safely-scoped]

namespace foo {

namespace {

using ::bar::something;

}  // anonymous namespace
}  // namespace foo

