#include "clang/Tooling/ASTDiff/ASTDiff.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static cl::OptionCategory ClangDiffCategory("clang-diff options");

static cl::opt<bool> PrintMatches("dump-matches",
                                  cl::desc("Print the matched nodes."),
                                  cl::init(false), cl::cat(ClangDiffCategory));

static cl::opt<std::string> SourcePath(cl::Positional, cl::desc("<source>"),
                                       cl::Required,
                                       cl::cat(ClangDiffCategory));

static cl::opt<std::string> DestinationPath(cl::Positional,
                                            cl::desc("<destination>"),
                                            cl::Optional,
                                            cl::cat(ClangDiffCategory));

static cl::opt<int> MaxSize("s", cl::desc("<maxsize>"), cl::Optional,
                            cl::init(-1), cl::cat(ClangDiffCategory));

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

static void printNode(raw_ostream &OS, diff::SyntaxTree &Tree,
                      diff::NodeId Id) {
  if (Id.isInvalid()) {
    OS << "None";
    return;
  }
  OS << Tree.getNode(Id).getTypeLabel();
  std::string Value = Tree.getNodeValue(Id);
  if (!Value.empty())
    OS << ": " << Value;
  OS << "(" << Id << ")";
}

static void printTree(raw_ostream &OS, diff::SyntaxTree &Tree) {
  for (diff::NodeId Id : Tree) {
    for (int I = 0; I < Tree.getNode(Id).Depth; ++I)
      OS << " ";
    printNode(OS, Tree, Id);
    OS << "\n";
  }
}

static void printDstChange(raw_ostream &OS, diff::ASTDiff &Diff,
                           diff::SyntaxTree &SrcTree, diff::SyntaxTree &DstTree,
                           diff::NodeId Dst) {
  const diff::Node &DstNode = DstTree.getNode(Dst);
  diff::NodeId Src = Diff.getMapped(DstTree, Dst);
  switch (DstNode.Change) {
  case diff::None:
    break;
  case diff::Delete:
    llvm_unreachable("The destination tree can't have deletions.");
  case diff::Update:
    OS << "Update ";
    printNode(OS, SrcTree, Src);
    OS << " to " << DstTree.getNodeValue(Dst) << "\n";
    break;
  case diff::Insert:
  case diff::Move:
  case diff::UpdateMove:
    if (DstNode.Change == diff::Insert)
      OS << "Insert";
    else if (DstNode.Change == diff::Move)
      OS << "Move";
    else if (DstNode.Change == diff::UpdateMove)
      OS << "Update and Move";
    OS << " ";
    printNode(OS, DstTree, Dst);
    OS << " into ";
    printNode(OS, DstTree, DstNode.Parent);
    OS << " at " << DstTree.findPositionInParent(Dst) << "\n";
    break;
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
  if (MaxSize != -1)
    Options.MaxSize = MaxSize;

  diff::SyntaxTree SrcTree(Src->getASTContext());
  diff::SyntaxTree DstTree(Dst->getASTContext());
  diff::ASTDiff Diff(SrcTree, DstTree, Options);

  for (diff::NodeId Dst : DstTree) {
    diff::NodeId Src = Diff.getMapped(DstTree, Dst);
    if (PrintMatches && Src.isValid()) {
      llvm::outs() << "Match ";
      printNode(llvm::outs(), SrcTree, Src);
      llvm::outs() << " to ";
      printNode(llvm::outs(), DstTree, Dst);
      llvm::outs() << "\n";
    }
    printDstChange(llvm::outs(), Diff, SrcTree, DstTree, Dst);
  }
  for (diff::NodeId Src : SrcTree) {
    if (Diff.getMapped(SrcTree, Src).isInvalid()) {
      llvm::outs() << "Delete ";
      printNode(llvm::outs(), SrcTree, Src);
      llvm::outs() << "\n";
    }
  }

  return 0;
}
