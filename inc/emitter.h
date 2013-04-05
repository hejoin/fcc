#include "operand.h"
#include "asm.h"

#include "stdio.h"

struct ast;

typedef struct emitterCtx {
    asmCtx* Asm;

    operand labelReturnTo;
    operand labelBreakTo;
} emitterCtx;

void emitter (const struct ast* Tree, FILE* File);
