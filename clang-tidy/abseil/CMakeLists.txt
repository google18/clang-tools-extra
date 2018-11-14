set(LLVM_LINK_COMPONENTS support)

add_clang_library(clangTidyAbseilModule
  AbseilTidyModule.cpp
  AliasFreeHeadersCheck.cpp
  AnonymousEnclosedAliasesCheck.cpp
  DurationDivisionCheck.cpp
  DurationFactoryFloatCheck.cpp
  FasterStrsplitDelimiterCheck.cpp
  NoInternalDependenciesCheck.cpp
  NoNamespaceCheck.cpp
  QualifiedAliasesCheck.cpp
  RedundantStrcatCallsCheck.cpp
  SafelyScopedCheck.cpp
  StrCatAppendCheck.cpp
  StringFindStartswithCheck.cpp

  LINK_LIBS
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangTidy
  clangTidyUtils
  clangTooling
  )
