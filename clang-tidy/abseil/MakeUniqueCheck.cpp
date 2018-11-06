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
  Finder->addMatcher(declStmt(hasDescendant(cxxConstructExpr(
        hasType(cxxRecordDecl(hasName("std::unique_ptr"))),
        argumentCountIs(1),
        hasArgument(0, cxxNewExpr().bind("new"))).bind("construct"))).bind("decl"),this);

  Finder->addMatcher(cxxMemberCallExpr(
        callee(cxxMethodDecl(
             hasName("reset"),
             ofClass(cxxRecordDecl(hasName("std::unique_ptr"))))),
        argumentCountIs(1),
        hasArgument(0, cxxNewExpr().bind("reset_new"))).bind("reset_call"),this);
}

void MakeUniqueCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedConstructor = Result.Nodes.getNodeAs<CXXConstructExpr>("construct");
  const auto *NewExpr = Result.Nodes.getNodeAs<CXXNewExpr>("new");
  const auto *Decl = Result.Nodes.getNodeAs<DeclStmt>("decl");
  
  if (MatchedConstructor) {
    SourceRange argRange = NewExpr->getDirectInitRange();
    const SourceManager *SM = Result.SourceManager;
    llvm::StringRef ref = Lexer::getSourceText(CharSourceRange::getCharRange(argRange), *SM, LangOptions());
    SourceRange nameRange(MatchedConstructor->getBeginLoc(), MatchedConstructor->getParenOrBraceRange().getBegin());
    llvm::StringRef nameRef = Lexer::getSourceText(CharSourceRange::getCharRange(nameRange), *SM, LangOptions());
    std::string NewText = "auto " + nameRef.str() + " = absl::make_unique<" + NewExpr->getAllocatedType().getAsString() + ">" + ref.str() + ")";
    
    diag(Decl->getBeginLoc(), "prefer absl::make_unique to constructing unique_ptr with new") 
        << FixItHint::CreateReplacement(
             CharSourceRange::getTokenRange(Decl->getBeginLoc(), argRange.getEnd().getLocWithOffset(1)), NewText);
  }

  const auto *MatchedReset = Result.Nodes.getNodeAs<CXXMemberCallExpr>("reset_call");
  const auto *ResetNew = Result.Nodes.getNodeAs<CXXNewExpr>("reset_new");
  if (MatchedReset) {
    const SourceManager *SM = Result.SourceManager;
    const Expr *ObjectArg = MatchedReset->getImplicitObjectArgument();
    llvm::StringRef objName = Lexer::getSourceText(CharSourceRange::getCharRange(ObjectArg->getBeginLoc(), MatchedReset->getExprLoc().getLocWithOffset(-1)), *SM, LangOptions());
    SourceRange argRange = ResetNew->getDirectInitRange();
    llvm::StringRef ref = Lexer::getSourceText(CharSourceRange::getCharRange(argRange), *SM, LangOptions());
    std::string NewText = objName.str() + " = absl::make_unique<" + ResetNew->getAllocatedType().getAsString() + ">" + ref.str() + ")";
    diag(ObjectArg->getExprLoc(), "prefer absl::make_unique to resetting unique_ptr with new") 
        << FixItHint::CreateReplacement(
             CharSourceRange::getTokenRange(ObjectArg->getEndLoc(), argRange.getEnd().getLocWithOffset(1)), NewText);
  }
}

} // namespace abseil
} // namespace tidy
} // namespace clang
