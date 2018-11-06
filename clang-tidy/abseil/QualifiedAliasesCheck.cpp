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
  Finder->addMatcher(usingDecl().bind("x"), this);
}

void QualifiedAliasesCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  const auto *MatchedDecl = Result.Nodes.getNodeAs<UsingDecl>("x");
  // NestedNameSpecifier *fullDeclName = MatchedDecl->getQualifier();
  // SourceLocation qualifierLoc = (MatchedDecl->getQualifierLoc()).getBeginLoc();
  // if ((fullDeclName->getAsIdentifier())->getName().startswith("::"))
  //   return;
  const NestedNameSpecifierLoc qualifiedLoc = MatchedDecl->getQualifierLoc();
  const SourceLocation nameFrontLoc = qualifiedLoc.getBeginLoc();

  const NestedNameSpecifier *nameFront = MatchedDecl->getQualifier();
  const IdentifierInfo *nameFrontIdent = nameFront->getAsIdentifier();
  if (MatchedDecl->getName().startswith("::"))
    return;
  diag(nameFrontLoc, "using declaration is not fully qualified") << FixItHint::CreateInsertion(nameFrontLoc, "::");
}

} // namespace abseil
} // namespace tidy
} // namespace clang
