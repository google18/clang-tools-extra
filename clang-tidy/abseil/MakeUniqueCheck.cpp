//===--- MakeUniqueCheck.cpp - clang-tidy ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <iostream>
#include <string>
#include "MakeUniqueCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace abseil {

void MakeUniqueCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(cxxConstructExpr(
        hasType(cxxRecordDecl(hasName("std::unique_ptr"))),
        argumentCountIs(1),
        hasArgument(0, cxxNewExpr().bind("new"))).bind("construct"),this);

  Finder->addMatcher(cxxMemberCallExpr(
        callee(cxxMethodDecl(
             hasName("reset"),
             ofClass(cxxRecordDecl(hasName("std::unique_ptr"))))),
        argumentCountIs(1),
        hasArgument(0, cxxNewExpr())).bind("reset_call"),this);
  
}

void MakeUniqueCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedConstructor = Result.Nodes.getNodeAs<CXXConstructExpr>("construct");
  const auto *NewExpr = Result.Nodes.getNodeAs<CXXNewExpr>("new");

  if (MatchedConstructor) {
    SourceRange argRange = NewExpr->getDirectInitRange();
    const SourceManager *SM = Result.SourceManager;
    llvm::StringRef ref = Lexer::getSourceText(CharSourceRange::getCharRange(argRange), *SM, LangOptions());
    std::string NewText = "auto VAR_NAME = absl::make_unique<" + NewExpr->getAllocatedType().getAsString() + ">" + ref.str() + ")";
    
    diag(MatchedConstructor->getBeginLoc(), "prefer absl::make_unique to constructing unique_ptr with new") 
        << FixItHint::CreateReplacement(
             CharSourceRange::getTokenRange(MatchedConstructor->getBeginLoc(), argRange.getEnd()), NewText);
  }

  const auto *MatchedReset = Result.Nodes.getNodeAs<CXXMemberCallExpr>("reset_call");
  if (MatchedReset != nullptr) {  
    diag(MatchedReset->getExprLoc(), "replace reset with absl::make_unique");
  }
}

} // namespace abseil
} // namespace tidy
} // namespace clang
