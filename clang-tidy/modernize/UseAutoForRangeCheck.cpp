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
  Finder->addMatcher(cxxForRangeStmt(hasLoopVariable(varDecl().bind("LoopVar")))
                         .bind("loopInit"),
                     this);
}

void UseAutoForRangeCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *RangeLoop = Result.Nodes.getNodeAs<CXXForRangeStmt>("loopInit");
  llvm::outs() << "test\n";

  if (RangeLoop) {
    const auto *LoopVar = Result.Nodes.getNodeAs<VarDecl>("LoopVar");
    if (LoopVar) {
      std::string DiagText = "Prefer auto in range based loop variable";
      std::string NewText = "auto " + LoopVar->getNameAsString();
      SourceLocation Target = LoopVar->getBeginLoc();
      diag(Target, DiagText) << FixItHint::CreateReplacement(
          CharSourceRange::getTokenRange(Target, LoopVar->getEndLoc()),
          NewText);
    }
  }
}

} // namespace modernize
} // namespace tidy
} // namespace clang
