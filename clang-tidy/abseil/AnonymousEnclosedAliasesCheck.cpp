//===--- AnonymousEnclosedAliasesCheck.cpp - clang-tidy -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "AnonymousEnclosedAliasesCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace abseil {

void AnonymousEnclosedAliasesCheck::registerMatchers(MatchFinder *Finder) {
  // We try to match two nodes: 
  // 1. anonymous namespace declarations,
  // 2. using declarations that are not inside an anonymous declaration
  Finder->addMatcher(
  	namespaceDecl(isAnonymous()).bind("anonymous_namespace"), this);
  Finder->addMatcher(
  	usingDecl(unless(hasAncestor(
  		namespaceDecl(isAnonymous())))).bind("using_decl"), this);
}


void AnonymousEnclosedAliasesCheck::check(const MatchFinder::MatchResult &Result) {
  
  const UsingDecl *MatchedUsingDecl = 
  	Result.Nodes.getNodeAs<UsingDecl>("using_decl");
  // If a potential using declaration is matched,
  if (MatchedUsingDecl) {
  	// and if an anonymous namespace declaration has already been found,
  	// the matched using declaration is a target, and we print out 
  	// the diagnostics for it. Otherwise, we add the using declaration
  	// to the vector containing all candidate using declarations.
  	if (AnonymousNamespaceDecl) {
  		diag(MatchedUsingDecl->getLocation(), 
  			"UsingDecl %0 should be in the anonymous namespace") 
  			<< MatchedUsingDecl;
		} else {
			MatchedUsingDecls.push_back(MatchedUsingDecl);
		}
		return;
  }
  // Otherwise, an anonymous namespace declaration is matched. In this case, 
  // all the previously matched namespace declarations in the vector 
  // CurrentUsingDecl are our targets, and we print out the 
  // diagnostics for all of them.
  AnonymousNamespaceDecl = 
  	Result.Nodes.getNodeAs<NamespaceDecl>("anonymous_namespace");
  for (const UsingDecl* CurrentUsingDecl: MatchedUsingDecls) {
  	diag(CurrentUsingDecl->getLocation(), 
  		"UsingDecl %0 should be in the anonymous namespace")
  		<< CurrentUsingDecl;
	}
}

} // namespace abseil
} // namespace tidy
} // namespace clang
