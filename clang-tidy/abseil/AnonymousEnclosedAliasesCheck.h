//===--- AnonymousEnclosedAliasesCheck.h - clang-tidy -----------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_ABSEIL_ANONYMOUSENCLOSEDALIASESCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_ABSEIL_ANONYMOUSENCLOSEDALIASESCHECK_H

#include "../ClangTidy.h"

namespace clang {
namespace tidy {
namespace abseil {

/// FIXME: Write a short description.
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/abseil-anonymous-enclosed-aliases.html
class AnonymousEnclosedAliasesCheck : public ClangTidyCheck {
public:
  AnonymousEnclosedAliasesCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace abseil
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_ABSEIL_ANONYMOUSENCLOSEDALIASESCHECK_H
