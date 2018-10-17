//===--- MakeUniqueCheck.cpp - clang-tidy ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MakeUniqueCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace abseil {

void MakeUniqueCheck::registerMatchers(MatchFinder *Finder) {
Finder->addMatcher(cxxConstructExpr(allOf(
        hasType( cxxRecordDecl(hasName("std::unique_ptr"))),
        hasArgument(0,cxxNewExpr()))).bind("x"),this);


  Finder->addMatcher(cxxMemberCallExpr(allOf(
        callee(cxxMethodDecl(
             hasName("reset"),
             ofClass(cxxRecordDecl(hasName("std::unique_ptr"), decl())))),
        hasArgument(0, cxxNewExpr()))).bind("y"),this);

}

void MakeUniqueCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  const auto *MatchedDecl = Result.Nodes.getNodeAs<CXXConstructExpr>("x");
  diag(MatchedDecl->getLocation(), "replace new with make_unique");

}

} // namespace abseil
} // namespace tidy
} // namespace clang
