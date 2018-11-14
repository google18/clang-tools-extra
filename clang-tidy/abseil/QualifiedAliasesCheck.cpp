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
  // Looks for all using declarations.
  Finder->addMatcher(usingDecl().bind("x"), this);
}

void QualifiedAliasesCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<UsingDecl>("x");

  // Finds the nested-name-specifier location.
  const NestedNameSpecifierLoc qualifiedLoc = MatchedDecl->getQualifierLoc();
  const SourceLocation nameFrontLoc = qualifiedLoc.getBeginLoc();

  // Ignores the using declaration if its fully qualified.
  const SourceManager *SM = Result.SourceManager;
  CharSourceRange frontRange = CharSourceRange();
  frontRange.setBegin(nameFrontLoc);
  frontRange.setEnd(nameFrontLoc.getLocWithOffset(2));
  llvm::StringRef ref = Lexer::getSourceText(frontRange, *SM, LangOptions());

  if (ref.startswith("::"))
    return;

  diag(nameFrontLoc, "using declaration is not fully qualified")
  << FixItHint::CreateInsertion(nameFrontLoc, "::");
}

} // namespace abseil
} // namespace tidy
} // namespace clang
