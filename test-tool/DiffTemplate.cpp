#include "clang/ASTMatchers/Dynamic/Registry.h"
#include "clang/Tooling/ASTDiff/ASTDiff.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"
#include <vector>
#include <stack>
#include <deque>
#include <iostream>
#include <fstream>

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

// Convert an ASTNode to a corresponding node matcher
// Simplifies this by lowercasing the first character
// TODO: ObjC matchers and other edge cases
std::string toMatcherName(llvm::StringRef TypeLabel) {
  if (TypeLabel.startswith(StringRef("CXX"))) {
    return "cxx" + TypeLabel.drop_front(3).str();
  }
  else {
    return std::string(1, tolower(TypeLabel[0])) + TypeLabel.drop_front(1).str();
  }
}

// Print a SyntaxTree through in-order traversal
// Useful for seeing what the tree looks like
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

// Given an Expr construct a matcher for the exact type
std::string exprTypeMatcher(const Expr* E) {
  std::string String;
  String += "hasType(cxxRecordDecl(hasName(\"";
  String += E->getType().getAsString();
  String += "\"))), ";
  return String;
}

// Creates matcher code for the arguments of a callExpr.
std::string callExprArgs(const CallExpr* CE){ 
  std::string MatchCode;

  //Args
  std::vector<const clang::Expr*> ArgVector; 
  CallExpr::const_arg_range Args = CE->arguments();
  if(Args.begin() != Args.end()){
    int i = 0;
    for(const clang::Expr* Arg : Args ){
      ArgVector.push_back(Arg);
      MatchCode += "callExpr(hasArgument(" + std::to_string(i) + ",";
      //MatchCode += Arg->getType().getAsString();
      MatchCode += "declRefExpr()))";
      ++i;
    }
  }
  return MatchCode;
}
//Creates matcher code for the callee of a call expr.
std::string callExprCallee(const CallExpr* CE){
  std::string MatchCode;
  const clang::FunctionDecl* dirCallee = CE->getDirectCallee();
  if(dirCallee){
    MatchCode += "callExpr(callee(";
    //might need to generalize
    MatchCode += "cxxMethodDecl(hasName(\"";
    MatchCode += dirCallee->getNameAsString()  + "\"))))";
  }
  return MatchCode;
}

// Recursively print the matcher for a Tree at the
// given NodeId root
void printMatcher(const diff::SyntaxTree& Tree,
                  const diff::NodeId& Id,
                  std::string& Builder) {

  // Get the Node object
  const diff::Node CurrNode = Tree.getNode(Id);

  // Simplest matcher for the node itself
  Builder += toMatcherName(CurrNode.getTypeLabel());
  Builder += "(";

  // Get the ASTNode in the AST
  ast_type_traits::DynTypedNode ASTNode = CurrNode.ASTNode;

  // Cast as Expr and try to match on it
  const Expr* E = ASTNode.get<Expr>();
  if (E) {
    Builder += exprTypeMatcher(E);
  }

  // TODO: ADD MORE NARROWING MATCHERS HERE
  const CallExpr* C = ASTNode.get<CallExpr>();
  if(C){
    Builder += callExprCallee(C);
    Builder += callExprArgs(C);
  }
  // Recurse through children
  for (diff::NodeId Child : CurrNode.Children) {
    Builder += "hasChild(";
    printMatcher(Tree, Child, Builder);
    Builder += "), ";
  }
  Builder += ")";
}

// Removes malformed comma patterns in the resulting matcher
// string
void cleanUpCommas(std::string& String) {
  size_t Pos = std::string::npos;
  while ((Pos = String.find(", )")) != std::string::npos) {
    String.erase(Pos, 2);
  }
}

// Find root-to-node path for lowest common ancestor
std::deque<diff::NodeId> findRootPath(const diff::SyntaxTree& Tree, 
                                      const diff::NodeId& Id) {
  std::deque<diff::NodeId> Deque;
  diff::NodeId CurrId = Id;
  Deque.push_front(CurrId);
  while (CurrId != Tree.getRootId()) {
    CurrId = Tree.getNode(CurrId).Parent;
    Deque.push_front(CurrId);
  }
  return Deque;
}

// Lowest common ancestor for multiple nodes in AST
diff::NodeId LCA(const diff::SyntaxTree& Tree, std::vector<diff::NodeId> Ids) {
  std::vector<std::deque<diff::NodeId>> Paths; 

  llvm::outs() << "Calculating root to node paths...\n";
  for (diff::NodeId Id : Ids) {
    Paths.push_back(findRootPath(Tree, Id));
  }

  for (std::deque<diff::NodeId> Path : Paths) {
    for (diff::NodeId NId : Path) {
      llvm::outs() << NId.Id << ", ";
    }
    llvm::outs() << "\n";
  }

  // LCA is bounded by length of shortest path
  size_t ShortestLength = Paths[0].size();
  for (size_t i = 0; i < Paths.size(); i++) {
    if (Paths[i].size() < ShortestLength) 
      ShortestLength = Paths[i].size();
  }

  llvm::outs() << "Iterating through paths...\n";

  // Iterate through paths until one differs
  size_t Idx;
  for (Idx = 0; Idx < ShortestLength; Idx++) {
    diff::NodeId CurrValue = Paths[0][Idx];
    for (size_t i = 0; i < Paths.size(); i++) {
      if (Paths[i][Idx] != CurrValue) {
        return Paths[0][Idx-1];
      }
    }
  }
 
  return Paths[0][ShortestLength-1];
}

// Find highest but most specific ancestor of given node
// This is where we bind the root of our matcher
diff::NodeId walkUpNode(const diff::SyntaxTree& Tree,
                        const diff::NodeId Id) {
  diff::NodeId CurrId = Id;
  while (CurrId != Tree.getRootId()) {
    diff::NodeId Parent = Tree.getNode(CurrId).Parent;
    diff::Node ParentNode = Tree.getNode(Parent);
    llvm::StringRef ParentType = ParentNode.getTypeLabel();
    if (ParentType.equals(llvm::StringRef("DeclStmt")) || 
        ParentType.equals(llvm::StringRef("CompoundStmt")) ||
        ParentType.equals(llvm::StringRef("TranslationUnitDecl"))) {
      return CurrId;
    }
    CurrId = Parent;
  }
  return CurrId;
}

// Utility for computing a list of diffs with respect to the 
// source SyntaxTree
std::vector<diff::NodeId> findSourceDiff(const diff::SyntaxTree& SrcTree,
                                         const diff::SyntaxTree& DstTree,
                                         const diff::ASTDiff& Diff) {
  std::vector<diff::NodeId> DiffNodes;
  for (diff::NodeId Dst : DstTree) {
    const diff::Node &DstNode = DstTree.getNode(Dst);
    if (DstNode.Change != diff::None && DstNode.Change != diff::Insert) {
      diff::NodeId Src = Diff.getMapped(DstTree, Dst);
      DiffNodes.push_back(Src);
    }
  }

  for (diff::NodeId Src : SrcTree) {
    if (Diff.getMapped(SrcTree, Src).isInvalid()) {
      DiffNodes.push_back(Src);
    }
  }
  return DiffNodes;
}

//writes a string to cpp file
void write2File(std::string in){
  std::ofstream file("matcherDump.cpp");
  file << in;
  file.close();
  //TODO Line wont work on non clinic machines
  system("/Users/clinic18/Desktop/llvm/build/bin/clang-format -i matcherDump.cpp");
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
  
  printInorder(SrcTree);
  llvm::outs() << "\n";
 
  std::vector<diff::NodeId> DiffNodes = findSourceDiff(SrcTree, DstTree, Diff);
  for (diff::NodeId Id : DiffNodes) {
    llvm::outs() << Id.Id << ", ";
  }
  llvm::outs() << "\n";

  llvm::outs() << "Computing LCA...\n";
  diff::NodeId Ancestor = LCA(SrcTree, DiffNodes);
  llvm::outs() << Ancestor.Id << "\n"; 

  llvm::outs() << "Walking up parents...\n";
  diff::NodeId DiffRoot = walkUpNode(SrcTree, Ancestor);
  llvm::outs() << DiffRoot.Id << "\n";

  std::string MatcherString;
  printMatcher(SrcTree, DiffRoot, MatcherString);
  cleanUpCommas(MatcherString);
  llvm::outs() << MatcherString << "\n";
  write2File(MatcherString);
  return 0;
}
