//===--- SafelyScopedCheck.cpp - clang-tidy -------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "SafelyScopedCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace abseil {

void SafelyScopedCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(usingDecl(eachOf( usingDecl(unless(hasParent(namespaceDecl()))),
        usingDecl(hasParent(namespaceDecl(forEach(namespaceDecl())))) )).bind("use"), this;
}

void SafelyScopedCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *MatchedDecl = Result.Nodes.getNodeAs<UsingDecl>("use");
  diag(MatchedDecl->getLocation(), "UsingDecl %0 should be in the innermost scope")
      << MatchedDecl;
}

} // namespace abseil
} // namespace tidy
} // namespace clang
