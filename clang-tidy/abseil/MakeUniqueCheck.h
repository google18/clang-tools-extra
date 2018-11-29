//===--- MakeUniqueCheck.h - clang-tidy -------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_ABSEIL_MAKEUNIQUECHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_ABSEIL_MAKEUNIQUECHECK_H

#include "../ClangTidy.h"

namespace clang {
namespace tidy {
namespace abseil {

/// Checks for unclear pointer ownership through constructing std::unique_ptr with 
/// a call to new, and recommends using absl::make_unique or absl::WrapUnique 
/// instead. Note that these are similar to the std::make_unique functions, but 
/// differ in how they handle factory constructors and brace initialization, 
/// choosing to defer to absl::WrapUnique.
class MakeUniqueCheck : public ClangTidyCheck {
private:
  std::string getArgs(const SourceManager *SM, const CXXNewExpr *NewExpr);
  std::string getType(const SourceManager *SM, const CXXNewExpr *NewExpr, const Expr *Outer);

public:
  MakeUniqueCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace abseil
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_ABSEIL_MAKEUNIQUECHECK_H
