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
  Finder->addMatcher(cxxForRangeStmt(
          hasLoopVariable(varDecl(unless(hasType(autoType()))).bind("loopVar")),
          hasRangeInit(declRefExpr(hasType(
              cxxRecordDecl(hasName("::std::map")).bind("loopRange")))))
      .bind("loopInit"),this);
}

void UseAutoForRangeCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *RangeLoop = Result.Nodes.getNodeAs<CXXForRangeStmt>("loopInit");
  if (RangeLoop) {
    const auto *LoopRange = Result.Nodes.getNodeAs<CXXRecordDecl>("loopRange");
    const auto *LoopVar = Result.Nodes.getNodeAs<VarDecl>("loopVar");
    if (LoopRange && LoopVar) {
      //need to check if the type is not auto
        auto VarType = LoopVar->getType();
        llvm::outs() << VarType.getAsString() << "\n";
        if(VarType.getAsString() != "auto"){ 
        std::string DiagText =
            "Prefer auto in range based loop over map object";
        std::string NewText = "auto " + LoopVar->getNameAsString();
        SourceLocation Target = LoopVar->getBeginLoc();
        diag(Target, DiagText) << FixItHint::CreateReplacement(
            CharSourceRange::getTokenRange(Target, LoopVar->getEndLoc()),
            NewText);
      }
    }
  }
}

} // namespace modernize
} // namespace tidy
} // namespace clang
