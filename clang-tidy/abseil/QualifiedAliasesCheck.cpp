//===--- QualifiedAliasesCheck.cpp - clang-tidy ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "QualifiedAliasesCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace abseil {

void QualifiedAliasesCheck::registerMatchers(MatchFinder *Finder) {
  // Matches all using declarations.
  Finder->addMatcher(usingDecl().bind("x"), this);
}

void QualifiedAliasesCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<UsingDecl>("x");

  // Finds the nested-name-specifier location.
  const NestedNameSpecifierLoc QualifiedLoc = MatchedDecl->getQualifierLoc();
  const SourceLocation FrontLoc = QualifiedLoc.getBeginLoc();
 
  // Checks if the using declaration is fully qualified.
  const SourceManager *SM = Result.SourceManager;
  CharSourceRange FrontRange = CharSourceRange();
  FrontRange.setBegin(FrontLoc);
  FrontRange.setEnd(FrontLoc.getLocWithOffset(2));
  llvm::StringRef Beg = Lexer::getSourceText(FrontRange, *SM, LangOptions());
  
  // If the using declaration is fully qualified, don't produce a warning.
  if (Beg.startswith("::"))
    return;

  diag(FrontLoc, "using declaration is not fully qualified: see "
  "https://abseil.io/tips/119");
}

} // namespace abseil
} // namespace tidy
} // namespace clang
