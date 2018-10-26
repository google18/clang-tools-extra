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
  Finder->addMatcher(cxxConstructExpr(
        hasType( cxxRecordDecl(hasName("std::unique_ptr"))),
        argumentCountIs(1),
        hasArgument(0,cxxNewExpr())).bind("x"),this);
  

  Finder->addMatcher(cxxMemberCallExpr(
        callee(cxxMethodDecl(
             hasName("reset"),
             ofClass(cxxRecordDecl(hasName("std::unique_ptr"))))),
        argumentCountIs(1),
        hasArgument(0, cxxNewExpr())).bind("y"),this);
  
}

void MakeUniqueCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedConstructor = Result.Nodes.getNodeAs<CXXConstructExpr>("x");
  if (MatchedConstructor != nullptr) {
    diag(MatchedConstructor->getLocation(), "replace new with absl::make_unique");
  }

  const auto *MatchedReset = Result.Nodes.getNodeAs<CXXMemberCallExpr>("y");
  if (MatchedReset != nullptr) {  
    diag(MatchedReset->getExprLoc(), "replace reset with absl::make_unique");
  }
}

} // namespace abseil
} // namespace tidy
} // namespace clang
