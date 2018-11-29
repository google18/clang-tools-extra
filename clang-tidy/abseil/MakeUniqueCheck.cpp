//===--- MakeUniqueCheck.cpp - clang-tidy ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <string>
#include "MakeUniqueCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace abseil {

std::string MakeUniqueCheck::getArgs(const SourceManager *SM,
                                     const CXXNewExpr *NewExpr) {
  if (NewExpr->getInitializationStyle() == CXXNewExpr::InitializationStyle::ListInit) {
    SourceRange InitRange = NewExpr->getInitializer()->getSourceRange();
    llvm::StringRef ArgRef = Lexer::getSourceText(CharSourceRange::getCharRange(InitRange.getBegin().getLocWithOffset(1), InitRange.getEnd()), *SM, LangOptions());
    return "(" + ArgRef.str() + ")";
  }
  llvm::StringRef ArgRef = Lexer::getSourceText(CharSourceRange::getCharRange(NewExpr->getDirectInitRange()), *SM, LangOptions());
  return (ArgRef.str().length() > 0) ? ArgRef.str() + ")" : "()";
}

std::string MakeUniqueCheck::getType(const SourceManager *SM,
                                     const CXXNewExpr *NewExpr,
                                     const Expr *Outer) {
  SourceRange TypeRange(NewExpr->getAllocatedTypeSourceInfo()->getTypeLoc().getBeginLoc(), NewExpr->getDirectInitRange().getBegin());
  if (!TypeRange.isValid()) {
    TypeRange.setEnd(Outer->getEndLoc());
  }
  llvm::StringRef TypeRef = Lexer::getSourceText(CharSourceRange::getCharRange(TypeRange), *SM, LangOptions());
  return TypeRef.str();
}

void MakeUniqueCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(cxxConstructExpr(
        hasType(cxxRecordDecl(hasName("std::unique_ptr"))),
        argumentCountIs(1),
        hasArgument(0, cxxNewExpr().bind("cons_new")),
        anyOf(hasParent(decl().bind("cons_decl")), anything())).bind("cons"),this);

  Finder->addMatcher(cxxMemberCallExpr(
        callee(cxxMethodDecl(
             hasName("reset"),
             ofClass(cxxRecordDecl(hasName("std::unique_ptr"))))),
        argumentCountIs(1),
        hasArgument(0, cxxNewExpr().bind("reset_new"))).bind("reset_call"),this);
}

void MakeUniqueCheck::check(const MatchFinder::MatchResult &Result) {
  const SourceManager *SM = Result.SourceManager;
  const auto *Cons = Result.Nodes.getNodeAs<CXXConstructExpr>("cons");
  const auto *ConsNew = Result.Nodes.getNodeAs<CXXNewExpr>("cons_new");
  const auto *ConsDecl = Result.Nodes.getNodeAs<Decl>("cons_decl");
  
  if (Cons) {
    // Get name of declared variable, if exists
    llvm::StringRef NameRef = Lexer::getSourceText(CharSourceRange::getCharRange(Cons->getBeginLoc(), Cons->getParenOrBraceRange().getBegin()), *SM, LangOptions());
    std::string Left = (ConsDecl) ? "auto " + NameRef.str() + " = " : "";

    std::string NewText;
    std::string DiagText;

    // Use WrapUnique for list initialization
    if (ConsNew->getInitializationStyle() == CXXNewExpr::InitializationStyle::ListInit) {
      NewText = Left + "absl::WrapUnique" + getArgs(SM, ConsNew);        
      DiagText = "prefer absl::WrapUnique to constructing unique_ptr with new";
    } else {
      NewText = Left + "absl::make_unique<" + getType(SM, ConsNew, Cons) + ">" +
                getArgs(SM, ConsNew);
      DiagText = "prefer absl::make_unique to constructing unique_ptr with new";
    }

    // If there is an associated Decl, start diagnostic there, otherwise use the beginning of the Expr   
    SourceLocation Target = (ConsDecl) ? ConsDecl->getBeginLoc() : Cons->getExprLoc(); 
    diag(Target, DiagText) 
        << FixItHint::CreateReplacement(
             CharSourceRange::getTokenRange(Target, Cons->getEndLoc()), NewText);
  }

  const auto *Reset = Result.Nodes.getNodeAs<CXXMemberCallExpr>("reset_call");
  const auto *ResetNew = Result.Nodes.getNodeAs<CXXNewExpr>("reset_new");
  if (Reset) {
    // Get name of caller object
    const Expr *ObjectArg = Reset->getImplicitObjectArgument();
    llvm::StringRef ObjName = Lexer::getSourceText(CharSourceRange::getCharRange(ObjectArg->getBeginLoc(), Reset->getExprLoc().getLocWithOffset(-1)), *SM, LangOptions());
    
    std::string NewText = ObjName.str() + " = absl::make_unique<" + getType(SM, ResetNew, Reset) + ">" + getArgs(SM, ResetNew);
    std::string DiagText = "prefer absl::make_unique to resetting unique_ptr with new";

    // Use WrapUnique for list initialization
    if (ResetNew->getInitializationStyle() == CXXNewExpr::InitializationStyle::ListInit) {
      NewText = ObjName.str() + " = absl::WrapUnique" + getArgs(SM, ResetNew);
      DiagText = "prefer absl::WrapUnique to resetting unique_ptr with new";
    } else {
      NewText = ObjName.str() + " = absl::make_unique<" +
                getType(SM, ResetNew, Reset) + ">" + getArgs(SM, ResetNew);
      DiagText = "prefer absl::make_unique to resetting unique_ptr with new";
    }

    diag(ObjectArg->getExprLoc(), DiagText) 
        << FixItHint::CreateReplacement(
             CharSourceRange::getTokenRange(ObjectArg->getExprLoc(), Reset->getEndLoc()), NewText);
  }
}

} // namespace abseil
} // namespace tidy
} // namespace clang
