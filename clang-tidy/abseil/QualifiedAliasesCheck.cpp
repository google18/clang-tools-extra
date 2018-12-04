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

#include <string>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace abseil {

void QualifiedAliasesCheck::registerMatchers(MatchFinder *Finder) {
  // Looks for all using declarations.
  Finder->addMatcher(usingDecl().bind("x"), this);
}

void QualifiedAliasesCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<UsingDecl>("x");

  // Finds the nested-name-specifier location.
  const NestedNameSpecifierLoc QualifiedLoc = MatchedDecl->getQualifierLoc();
  const SourceLocation FrontLoc = QualifiedLoc.getBeginLoc();

  // Ignores the using declaration if its fully qualified.
  const SourceManager *SM = Result.SourceManager;
  CharSourceRange FrontRange = CharSourceRange();
  frontRange.setBegin(FrontLoc);
  frontRange.setEnd(FrontLoc.getLocWithOffset(2));
  llvm::StringRef Beg = Lexer::getSourceText(FrontRange, *SM, LangOptions());

  if (Beg.startswith("::"))
    return;
 
  diag(FrontLoc, "using declaration is not fully qualified");
}

} // namespace abseil
} // namespace tidy
} // namespace clang
