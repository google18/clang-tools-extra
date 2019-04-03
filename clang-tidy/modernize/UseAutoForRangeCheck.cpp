//===--- UseAutoForRangeCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UseAutoForRangeCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <string>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace modernize {

void UseAutoForRangeCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      cxxForRangeStmt(has(varDecl())).bind("loopInit"), this);
}

void UseAutoForRangeCheck::check(const MatchFinder::MatchResult &Result) {
  const SourceManager *SM = Result.SourceManager;
  const auto *VarDec = Result.Nodes.getNodeAs<VarDecl>("loopInit");
  if(VarDec){
    std::string DiagText = "TODO";
    llvm::StringRef ObjName = Lexer::getSourceText(CharSourceRange::getCharRange(VarDec->getSourceRange()), *SM, LangOptions());
    std::string NewText = ObjName.str();
    llvm::outs() << NewText << "\n"; 
  
  }
  
}

} // namespace modernize
} // namespace tidy
} // namespace clang
