//===--- WrapUniqueCheck.cpp - clang-tidy ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <string>
#include <iostream>
#include "WrapUniqueCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace abseil {

std::string WrapUniqueCheck::getArgs(const SourceManager *SM,
                                     const CallExpr *MemExpr) {
  std::cout << MemExpr->getNumArgs() << std::endl;
  
  
  llvm::StringRef ArgRef = Lexer::getSourceText(
    CharSourceRange::getCharRange(
      MemExpr->getSourceRange()), *SM, LangOptions());

  return (ArgRef.str().length() > 0) ? ArgRef.str() + ")" : "()";
}

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
    hasArgument(0, callExpr().bind("cons_new"))
    
).bind("facConstructor"), this);

  Finder->addMatcher(
  cxxConstructExpr(
    has(callExpr())
).bind("ex4"),this);

}


void WrapUniqueCheck::check(const MatchFinder::MatchResult &Result) {
  // gets the instance of factory constructor
  const SourceManager *SM = Result.SourceManager;
  const auto *facExpr = Result.Nodes.getNodeAs<CXXMemberCallExpr>("facConstructor");
  const auto *ConsNew = Result.Nodes.getNodeAs<CallExpr>("cons_new");
  //upfc.reset(FactoryConstruvtible::NewFC());

  //upfc = absl::WrapUnique(FactoryConstructible::NewFC());

  if(facExpr){
    std::string diagText = "Perfer absl::WrapUnique for reseting unique_ptr";
    std::string newText;
    
    const Expr *ObjectArg = facExpr -> getImplicitObjectArgument();

    SourceLocation Target = ObjectArg ->  getExprLoc();
    
    llvm::StringRef ObjName = Lexer::getSourceText(CharSourceRange::getCharRange(
        ObjectArg->getBeginLoc(), facExpr->getExprLoc().getLocWithOffset(-1)),
        *SM, LangOptions());

    newText = ObjName.str() + " = absl::WrapUnique(" + getArgs(SM, ConsNew) + ")"; 

    diag(Target, diagText)
      << FixItHint::CreateReplacement(
           CharSourceRange::getTokenRange( 
           Target, facExpr->getEndLoc()), newText);
  }

}

} // namespace abseil
} // namespace tidy
} // namespace clang 
