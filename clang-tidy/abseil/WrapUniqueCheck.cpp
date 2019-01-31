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
#include <string>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace abseil {

std::string WrapUniqueCheck::getArgs(const SourceManager *SM,
                                     const CallExpr *MemExpr) {
  llvm::StringRef ArgRef = Lexer::getSourceText(
      CharSourceRange::getCharRange(MemExpr->getSourceRange()), *SM,
      LangOptions());

  return (ArgRef.str().length() > 0) ? ArgRef.str() + ")" : "()";
}

void WrapUniqueCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      cxxMemberCallExpr(
          callee(cxxMethodDecl(
              hasName("reset"),
              ofClass(cxxRecordDecl(hasName("std::unique_ptr"), decl())))),
          has(memberExpr(has(declRefExpr()))),
          has(callExpr(has(implicitCastExpr(has(declRefExpr()))))),
          hasArgument(0, callExpr().bind("callExpr")))
          .bind("facCons"),
      this);

  Finder->addMatcher(
      cxxConstructExpr(anyOf(hasParent(decl().bind("cons_decl")), anything()),
                       hasType(cxxRecordDecl(hasName("std::unique_ptr"))),
                       has(callExpr().bind("FC_call")))
          .bind("upfc"),
      this);
}

void WrapUniqueCheck::check(const MatchFinder::MatchResult &Result) {
  // gets the instance of factory constructor
  const SourceManager *SM = Result.SourceManager;
  const auto *facExpr = Result.Nodes.getNodeAs<CXXMemberCallExpr>("facCons");
  const auto *callExpr = Result.Nodes.getNodeAs<CallExpr>("callExpr");

  const auto *cons = Result.Nodes.getNodeAs<CXXConstructExpr>("upfc");
  const auto *consDecl = Result.Nodes.getNodeAs<Decl>("cons_decl");
  const auto *FC_Call = Result.Nodes.getNodeAs<CallExpr>("FC_call");

  if (facExpr) {
    std::string diagText = "Perfer absl::WrapUnique for resetting unique_ptr";
    std::string newText;

    const Expr *ObjectArg = facExpr->getImplicitObjectArgument();
    SourceLocation Target = ObjectArg->getExprLoc();
    llvm::StringRef ObjName =
        Lexer::getSourceText(CharSourceRange::getCharRange(
                                 ObjectArg->getBeginLoc(),
                                 facExpr->getExprLoc().getLocWithOffset(-1)),
                             *SM, LangOptions());

    newText =
        ObjName.str() + " = absl::WrapUnique(" + getArgs(SM, callExpr) + ")";

    diag(Target, diagText) << FixItHint::CreateReplacement(
        CharSourceRange::getTokenRange(Target, facExpr->getEndLoc()), newText);
  }

  if (cons) {
    if (cons->isListInitialization()) {
      return;
    }

    std::string diagText = "Perfer absl::WrapUnique to constructing unique_ptr";
    std::string newText;
    std::string Left;

    llvm::StringRef NameRef = Lexer::getSourceText(
        CharSourceRange::getCharRange(cons->getBeginLoc(),
                                      cons->getParenOrBraceRange().getBegin()),
        *SM, LangOptions());

    Left = (consDecl) ? "auto " + NameRef.str() + " = " : "";
    newText = Left + "absl::WrapUnique(" + getArgs(SM, FC_Call) + ")";
    SourceLocation Target =
        (consDecl) ? consDecl->getBeginLoc() : cons->getExprLoc();

    diag(Target, diagText) << FixItHint::CreateReplacement(
        CharSourceRange::getTokenRange(Target, cons->getEndLoc()), newText);
  }
}
} // namespace abseil
} // namespace tidy
} // namespace clang
