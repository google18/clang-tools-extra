
void WrapUniqueCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      cxxMemberCallExpr(
          callee(cxxMethodDecl(
              hasName("reset"),
              ofClass(cxxRecordDecl(hasName("std::unique_ptr"), decl())))),
          has(memberExpr(has(declRefExpr()))),
          has(callExpr(has(implicitCastExpr(has(declRefExpr()))))),
          hasArgument(0, callExpr().bind("callExpr")))
          .bind("facCons"),
      this);
  Finder->addMatcher(
      cxxConstructExpr(anyOf(hasParent(decl().bind("cons_decl")), anything()),
                       hasType(cxxRecordDecl(hasName("std::unique_ptr"))),
                       has(callExpr().bind("FC_call")))
          .bind("upfc"),
      this);



varDecl(
  has(
    cxxConstructExpr(
      hasType(cxxRecordDecl(hasName("unique_ptr"))), 
      argumentCountIs(1), 
      hasArgument(0, expr()), 
      has(
        cxxNewExpr(
          has(
            integerLiteral()
          )
        )
      )
    )
  )
);


cxxConstructExpr(
    hasType(cxxRecordDecl(hasName("std::unique_ptr"))),
    argumentCountIs(1),
    hasArgument(0, cxxNewExpr().bind("cons_new"))
);

int main() {
  int a = 5;
  int b = 2 * a;
  return 0;
}

binaryOperator(
  allOf(
    implicitCastExpr(
      has(
        declRefExpr()
      )
    ),
    implicitCastExpr(
      has(
        declRefExpr()
      )
    )
  )
)


