//===--- WrapUniqueCheck.cpp - clang-tidy ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

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
  ,
    
).bind("facConstructor"), this);
}


void WrapUniqueCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  const auto *MatchedDecl = Result.Nodes.getNodeAs<FunctionDecl>("x");
  if (MatchedDecl->getName().startswith("awesome_"))
    return;
  diag(MatchedDecl->getLocation(), "function %0 is insufficiently awesome")
      << MatchedDecl
      << FixItHint::CreateInsertion(MatchedDecl->getLocation(), "awesome_");
}

} // namespace abseil
} // namespace tidy
} // namespace clang
