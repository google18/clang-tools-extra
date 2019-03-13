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

static std::string getArgs(const SourceManager *SM,
                                     const CallExpr *MemExpr) {
  llvm::StringRef ArgRef = Lexer::getSourceText(
      CharSourceRange::getCharRange(MemExpr->getSourceRange()), *SM,
      LangOptions());

  return (!ArgRef.empty()) ? ArgRef.str() + ")" : "()";
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
  const auto *FacExpr = Result.Nodes.getNodeAs<CXXMemberCallExpr>("facCons");
  const auto *Cons = Result.Nodes.getNodeAs<CXXConstructExpr>("upfc");
  if (FacExpr) {
    const auto *CExpr = Result.Nodes.getNodeAs<CallExpr>("callExpr");
    std::string DiagText = "Perfer absl::WrapUnique for resetting unique_ptr";

    const Expr *ObjectArg = FacExpr->getImplicitObjectArgument();
    SourceLocation Target = ObjectArg->getExprLoc();
    llvm::StringRef ObjName =
        Lexer::getSourceText(CharSourceRange::getCharRange(
                                 ObjectArg->getBeginLoc(),
                                 FacExpr->getExprLoc().getLocWithOffset(-1)),
                             *SM, LangOptions());

    std::string NewText =
        ObjName.str() + " = absl::WrapUnique(" + getArgs(SM, CExpr) + ")";

    diag(Target, DiagText) << FixItHint::CreateReplacement(
        CharSourceRange::getTokenRange(Target, FacExpr->getEndLoc()), NewText);
  }

  if (Cons) {
    if (Cons->isListInitialization()) {
      return;
    }

    const auto *FcCall = Result.Nodes.getNodeAs<CallExpr>("FC_call");
    const auto *ConsDecl = Result.Nodes.getNodeAs<Decl>("cons_decl");
    std::string DiagText = "Perfer absl::WrapUnique to constructing unique_ptr";

    llvm::StringRef NameRef = Lexer::getSourceText(
        CharSourceRange::getCharRange(Cons->getBeginLoc(),
                                      Cons->getParenOrBraceRange().getBegin()),
        *SM, LangOptions());

    std::string Left = (ConsDecl) ? "auto " + NameRef.str() + " = " : "";
    std::string NewText =
        Left + "absl::WrapUnique(" + getArgs(SM, FcCall) + ")";
    SourceLocation Target =
        (ConsDecl) ? ConsDecl->getBeginLoc() : Cons->getExprLoc();

    diag(Target, DiagText) << FixItHint::CreateReplacement(
        CharSourceRange::getTokenRange(Target, Cons->getEndLoc()), NewText);
  }
}
} // namespace abseil
} // namespace tidy
} // namespace clang
