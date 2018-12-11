//===--- AliasFreeHeadersCheck.cpp - clang-tidy ---------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "AliasFreeHeadersCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace abseil {

void AliasFreeHeadersCheck::registerMatchers(MatchFinder *Finder) {
  // Match all using declarations in header files.
  Finder->addMatcher(usingDecl(isExpansionInFileMatching(".*\\.h.*")).bind("x"),
    this);
}

void AliasFreeHeadersCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<UsingDecl>("x");
  diag(MatchedDecl->getLocation(), "using declarations should not be included in header files");
}

} // namespace abseil
} // namespace tidy
} // namespace clang
