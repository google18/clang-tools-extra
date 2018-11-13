// RUN: %check_clang_tidy %s abseil-safely-scoped-aliases %t
namespace bar {

class something {

};
}

namespace foo {

using bar::something;

namespace {

}  // anonymous namespace
}  // namespace foo
// CHECK-MESSAGES: :[[@LINE-6]]:12: warning: UsingDecl 'something' should be in the innermost scope [abseil-safely-scoped-aliases]

// FIXME: Verify the applied fix.

// FIXME: Add something that doesn't trigger the check here.
namespace foo {

namespace {

using ::bar::something;

}  // anonymous namespace
}  // namespace foo
