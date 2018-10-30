set(LLVM_LINK_COMPONENTS support)

add_clang_library(clangTidyAbseilModule
  AbseilTidyModule.cpp
  AliasFreeHeadersCheck.cpp
  DurationDivisionCheck.cpp
  FasterStrsplitDelimiterCheck.cpp
  MakeUniqueCheck.cpp
  NoInternalDependenciesCheck.cpp
  NoNamespaceCheck.cpp
  RedundantStrcatCallsCheck.cpp
  SafelyScopedAliasesCheck.cpp
  StrCatAppendCheck.cpp
  StringFindStartswithCheck.cpp

  LINK_LIBS
  clangAST
  clangASTMatchers
  clangBasic
  clangLex
  clangTidy
  clangTidyUtils
  )
