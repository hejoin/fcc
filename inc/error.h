#include "sym.h"

struct type;
struct ast;

struct parserCtx;
struct analyzerCtx;

void errorExpected (struct parserCtx* ctx, const char* expected);
void errorUndefSym (struct parserCtx* ctx);
void errorUndefType (struct parserCtx* ctx);
void errorIllegalOutside (struct parserCtx* ctx, const char* what, const char* where);
void errorRedeclaredSymAs (struct parserCtx* ctx, const sym* Symbol, symTag tag);
void errorReimplementedSym (struct parserCtx* ctx, const sym* Symbol);
void errorFileNotFound (struct parserCtx* ctx, const char* name);

void errorTypeExpected (struct analyzerCtx* ctx, const struct ast* Node,
                        const char* where, const char* expected);
void errorTypeExpectedType (struct analyzerCtx* ctx, const struct ast* Node,
                            const char* where, const struct type* expected);
void errorLvalue (struct analyzerCtx* ctx, const struct ast* Node, opTag o);
void errorMismatch (struct analyzerCtx* ctx, const struct ast* Node, opTag o);
void errorDegree (struct analyzerCtx* ctx, const struct ast* Node,
                  const char* thing, int expected, int found, const char* where);
void errorMember (struct analyzerCtx* ctx, const struct ast* Node, const char* field);
void errorParamMismatch (struct analyzerCtx* ctx, const struct ast* Node,
                         const struct ast* fn, int n, const struct type* expected, const struct type* found);
void errorInitMismatch (struct analyzerCtx* ctx, const struct ast* variable, const struct ast* init);
void errorInitFieldMismatch (struct analyzerCtx* ctx, const struct ast* Node,
                             const sym* structSym, const sym* fieldSym);
void errorConflictingDeclarations (struct analyzerCtx* ctx, const struct ast* Node,
                                   const sym* Symbol, const struct type* found);
void errorRedeclared (struct analyzerCtx* ctx, const struct ast* Node, const sym* Symbol);
void errorAlreadyConst (struct analyzerCtx* ctx, const struct ast* Node);
void errorIllegalConst (struct analyzerCtx* ctx, const struct ast* Node);
void errorIllegalSymAsValue (struct analyzerCtx* ctx, const struct ast* Node, const sym* Symbol);
void errorCompileTimeKnown (struct analyzerCtx* ctx, const struct ast* Node,
                            const sym* Symbol, const char* what);
void errorCompoundLiteralWithoutType (struct analyzerCtx* ctx, const struct ast* Node);
void errorIncompletePtr (struct analyzerCtx* ctx, const struct ast* Node, opTag o);
void errorIncompleteDecl (struct analyzerCtx* ctx, const struct ast* Node);
void errorIncompleteParamDecl (struct analyzerCtx* ctx, const struct ast* Node, const struct ast* fn, int n);
void errorIncompleteReturnDecl (struct analyzerCtx* ctx, const struct ast* Node, const struct type* dt);
void errorConstAssignment (struct analyzerCtx* ctx, const struct ast* Node, opTag o);
