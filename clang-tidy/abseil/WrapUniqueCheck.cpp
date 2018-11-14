//===--- WrapUniqueCheck.cpp - clang-tidy ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include "WrapUniqueCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace abseil {

void WrapUniqueCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher( 
  cxxMemberCallExpr(
    callee(cxxMethodDecl(
      hasName("reset"),
      ofClass(cxxRecordDecl(hasName("std::unique_ptr"), decl()))))
  ,
    has(memberExpr(
      has(declRefExpr())))
  ,
    has(callExpr(
      has(implicitCastExpr(
        has(declRefExpr())))))
    
).bind("facConstructor"), this);

  Finder->addMatcher(
  cxxConstructExpr(
    has(callExpr())
).bind("ex4"),this);

}


void WrapUniqueCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  const auto *MatchedExpr = Result.Nodes.getNodeAs<CallExpr>("ex4");

  //std::cout << MatchedExpr->getCallReturnType() << std::newl;

  //diag(MatchedDecl->getLocation(), "function %0 is insufficiently awesome")
     // << MatchedDecl
     // << FixItHint::CreateInsertion(MatchedDecl->getLocation(), "awesome_");
}

} // namespace abseil
} // namespace tidy
} // namespace clang
