#include "clang/Tooling/ASTDiff/ASTDiff.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <vector>
#include <stack>

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static cl::OptionCategory ClangDiffCategory("clang-diff options");

static cl::opt<std::string> SourcePath(cl::Positional, cl::desc("<source>"),
                                       cl::Required,
                                       cl::cat(ClangDiffCategory));

static cl::opt<std::string> DestinationPath(cl::Positional,
                                            cl::desc("<destination>"),
                                            cl::Optional,
                                            cl::cat(ClangDiffCategory));

static cl::opt<std::string> BuildPath("p", cl::desc("Build path"), cl::init(""),
                                      cl::Optional, cl::cat(ClangDiffCategory));

static cl::list<std::string> ArgsAfter(
    "extra-arg",
    cl::desc("Additional argument to append to the compiler command line"),
    cl::cat(ClangDiffCategory));

static cl::list<std::string> ArgsBefore(
    "extra-arg-before",
    cl::desc("Additional argument to prepend to the compiler command line"),
    cl::cat(ClangDiffCategory));

static void addExtraArgs(std::unique_ptr<CompilationDatabase> &Compilations) {
  if (!Compilations)
    return;
  auto AdjustingCompilations =
      llvm::make_unique<ArgumentsAdjustingCompilations>(
          std::move(Compilations));
  AdjustingCompilations->appendArgumentsAdjuster(
      getInsertArgumentAdjuster(ArgsBefore, ArgumentInsertPosition::BEGIN));
  AdjustingCompilations->appendArgumentsAdjuster(
      getInsertArgumentAdjuster(ArgsAfter, ArgumentInsertPosition::END));
  Compilations = std::move(AdjustingCompilations);
}

static std::unique_ptr<ASTUnit>
getAST(const std::unique_ptr<CompilationDatabase> &CommonCompilations,
       const StringRef Filename) {
  std::string ErrorMessage;
  std::unique_ptr<CompilationDatabase> Compilations;
  if (!CommonCompilations) {
    Compilations = CompilationDatabase::autoDetectFromSource(
        BuildPath.empty() ? Filename : BuildPath, ErrorMessage);
    if (!Compilations) {
      llvm::errs()
          << "Error while trying to load a compilation database, running "
             "without flags.\n"
          << ErrorMessage;
      Compilations =
          llvm::make_unique<clang::tooling::FixedCompilationDatabase>(
              ".", std::vector<std::string>());
    }
  }
  addExtraArgs(Compilations);
  std::array<std::string, 1> Files = {{Filename}};
  ClangTool Tool(Compilations ? *Compilations : *CommonCompilations, Files);
  std::vector<std::unique_ptr<ASTUnit>> ASTs;
  Tool.buildASTs(ASTs);
  if (ASTs.size() != Files.size())
    return nullptr;
  return std::move(ASTs[0]);
}

struct NodeDiff {
  diff::ChangeKind Change;
  diff::NodeId Dst;
  diff::NodeId Src;
};

std::string diffToString(NodeDiff Diff) {
  std::string Str;
  switch(Diff.Change) {
  case diff::None:
    Str = "None";
    break;
  case diff::Delete:
    Str = "Delete " + std::to_string(Diff.Dst.Id);
    break;
  case diff::Update:
    Str = "Update " + std::to_string(Diff.Src.Id) + " to " + std::to_string(Diff.Dst.Id);
    break;
  case diff::Insert:
    Str = "Insert " + std::to_string(Diff.Dst.Id);
    break;
  case diff::Move:
    Str = "Move " + std::to_string(Diff.Dst.Id);
    break;
  case diff::UpdateMove:
    Str = "Update and Move " + std::to_string(Diff.Dst.Id);
    break;
  }
  return Str;
}

void printInorder(const diff::SyntaxTree& Tree) {
  std::stack<diff::NodeId> Stack;
  Stack.push(Tree.getRootId());
  while (!Stack.empty()) {
    const diff::NodeId CurrId = Stack.top();
    Stack.pop();
    const diff::Node CurrNode = Tree.getNode(CurrId);
    llvm::outs() << CurrNode.getTypeLabel();
    std::string Value = Tree.getNodeValue(CurrId);
    if (!Value.empty())
      llvm::outs() << ": " << Value;
    llvm::outs() << " (" << CurrId << ")\n";
    for (diff::NodeId Child : CurrNode.Children) {
      Stack.push(Child);
    }
  }
}

int main(int argc, const char **argv) {
  std::string ErrorMessage;
  std::unique_ptr<CompilationDatabase> CommonCompilations =
      FixedCompilationDatabase::loadFromCommandLine(argc, argv, ErrorMessage);
  if (!CommonCompilations && !ErrorMessage.empty())
    llvm::errs() << ErrorMessage;
  cl::HideUnrelatedOptions(ClangDiffCategory);
  if (!cl::ParseCommandLineOptions(argc, argv)) {
    cl::PrintOptionValues();
    return 1;
  }

  addExtraArgs(CommonCompilations);

  if (DestinationPath.empty()) {
    llvm::errs() << "Error: Exactly two paths are required.\n";
    return 1;
  }

  std::unique_ptr<ASTUnit> Src = getAST(CommonCompilations, SourcePath);
  std::unique_ptr<ASTUnit> Dst = getAST(CommonCompilations, DestinationPath);
  if (!Src || !Dst)
    return 1;

  diff::ComparisonOptions Options;

  diff::SyntaxTree SrcTree(Src->getASTContext());
  diff::SyntaxTree DstTree(Dst->getASTContext());
  diff::ASTDiff Diff(SrcTree, DstTree, Options);

  std::vector<NodeDiff> Diffs; 
  printInorder(SrcTree);
  llvm::outs() << "\n";
  printInorder(DstTree);
  llvm::outs() << "\n";
  
  for (diff::NodeId Dst : DstTree) {
    const diff::Node &DstNode = DstTree.getNode(Dst);
    if (DstNode.Change != diff::None) {
      diff::NodeId Src = Diff.getMapped(DstTree, Dst);
      NodeDiff DstDiff = {DstNode.Change, Dst, Src};
      Diffs.push_back(DstDiff);
    }
  }
  for (diff::NodeId Src : SrcTree) {
    if (Diff.getMapped(SrcTree, Src).isInvalid()) {
      NodeDiff SrcDiff = {diff::Delete, Src, diff::NodeId(0)};
      Diffs.push_back(SrcDiff);
    }
  }
  
  for (NodeDiff NDiff : Diffs) {
    llvm::outs() << diffToString(NDiff) << "\n";
  }

  return 0;
}
