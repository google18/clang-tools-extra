//===--- MakeUniqueCheck.h - clang-tidy -------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_ABSEIL_MAKEUNIQUECHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_ABSEIL_MAKEUNIQUECHECK_H

#include "../ClangTidy.h"
#include "../modernize/MakeSmartPtrCheck.h"

namespace clang {
namespace tidy {
namespace abseil {

/// Replace the pattern:
/// \code 
///   std::unique_ptr<type>(new type(args...))
/// \endcode
///
/// With the Abseil version:
/// \code 
///   absl::make_unique<type>(args...)
/// \endcode 
class MakeUniqueCheck : public modernize::MakeSmartPtrCheck {
public:
  MakeUniqueCheck(StringRef Name, ClangTidyContext *Context);

protected:
  SmartPtrTypeMatcher getSmartPointerTypeMatcher() const override;
  
  bool isLanguageVersionSupported(const LangOptions &LangOpts) const override;
};

} // namespace abseil
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_ABSEIL_MAKEUNIQUECHECK_H
