#include "../inc/parser-helpers.h"

#include "../inc/debug.h"
#include "../inc/sym.h"
#include "../inc/ast.h"
#include "../inc/error.h"

#include "../inc/parser.h"

#include "stdlib.h"
#include "stdarg.h"
#include "string.h"

static char* tokenTagGetStr (tokenTag Token);

/*:::: SCOPE ::::*/

sym* scopeSet (parserCtx* ctx, sym* Scope) {
    sym* Old = ctx->scope;
    ctx->scope = Scope;
    return Old;
}

/*:::: TOKEN HANDLING ::::*/

bool tokenIsKeyword (const parserCtx* ctx, keywordTag keyword) {
    /*Keyword is keywordUndefined if not keyword token*/
    return ctx->lexer->keyword == keyword;
}

bool tokenIsPunct (const parserCtx* ctx, punctTag punct) {
    return ctx->lexer->punct == punct;
}

bool tokenIsIdent (const parserCtx* ctx) {
    return ctx->lexer->token == tokenIdent;
}

bool tokenIsInt (const parserCtx* ctx) {
    return ctx->lexer->token == tokenInt;
}

bool tokenIsString (const parserCtx* ctx) {
    return ctx->lexer->token == tokenStr;
}

bool tokenIsDecl (const parserCtx* ctx) {
    sym* Symbol = symFind(ctx->scope, ctx->lexer->buffer);

    return    (Symbol && Symbol->tag != symId
                      && Symbol->tag != symParam)
           || tokenIsKeyword(ctx, keywordConst)
           || tokenIsKeyword(ctx, keywordAuto) || tokenIsKeyword(ctx, keywordStatic)
           || tokenIsKeyword(ctx, keywordExtern) || tokenIsKeyword(ctx, keywordTypedef)
           || tokenIsKeyword(ctx, keywordStruct) || tokenIsKeyword(ctx, keywordUnion)
           || tokenIsKeyword(ctx, keywordEnum)
           || tokenIsKeyword(ctx, keywordVoid) || tokenIsKeyword(ctx, keywordBool)
           || tokenIsKeyword(ctx, keywordChar) || tokenIsKeyword(ctx, keywordInt);
}

void tokenNext (parserCtx* ctx) {
    lexerNext(ctx->lexer);
    ctx->location = (tokenLocation) {ctx->filename,
                                     ctx->lexer->line,
                                     ctx->lexer->lineChar};
}

void tokenSkipMaybe (parserCtx* ctx) {
    /*if (!tokenIs(ctx, ")") && !tokenIs(ctx, "]") && !tokenIs(ctx, "}"))*/
        tokenNext(ctx);
}

void tokenMatch (parserCtx* ctx) {
    debugMsg("matched:%d:%d: '%s'", ctx->location.line, ctx->location.lineChar, ctx->lexer->buffer);
    tokenNext(ctx);
}

char* tokenDupMatch (parserCtx* ctx) {
    char* Old = strdup(ctx->lexer->buffer);

    tokenMatch(ctx);

    return Old;
}

/**
 * Return the string associated with a token tag
 */
static char* tokenTagGetStr (tokenTag tag) {
    if (tag == tokenUndefined) return "<undefined>";
    else if (tag == tokenOther) return "other";
    else if (tag == tokenEOF) return "end of file";
    else if (tag == tokenKeyword) return "keyword";
    else if (tag == tokenIdent) return "identifier";
    else if (tag == tokenInt) return "integer";
    else if (tag == tokenStr) return "string";
    else if (tag == tokenChar) return "character";
    else {
        char* str = malloc(logi(tag, 10)+2);
        sprintf(str, "%d", tag);
        debugErrorUnhandled("tokenTagGetStr", "token tag", str);
        free(str);
        return "<unhandled>";
    }
}

void tokenMatchKeyword (parserCtx* ctx, keywordTag keyword) {
    if (tokenIsKeyword(ctx, keyword))
        tokenMatch(ctx);

    else {
        const char* kw = keywordTagGetStr(keyword);
        char* kwInQuotes = malloc(strlen(kw)+3);
        sprintf(kwInQuotes, "'%s'", kw);
        errorExpected(ctx, kwInQuotes);
        free(kwInQuotes);

        tokenSkipMaybe(ctx);
    }
}

bool tokenTryMatchKeyword (parserCtx* ctx, keywordTag keyword) {
    if (tokenIsKeyword(ctx, keyword)) {
        tokenMatch(ctx);
        return true;

    } else
        return false;
}

void tokenMatchPunct (parserCtx* ctx, punctTag punct) {
    if (tokenIsPunct(ctx, punct))
        tokenMatch(ctx);

    else {
        const char* p = punctTagGetStr(punct);
        char* pInQuotes = malloc(strlen(p)+3);
        sprintf(pInQuotes, "'%s'", p);
        errorExpected(ctx, pInQuotes);
        free(pInQuotes);

        tokenSkipMaybe(ctx);
    }
}

bool tokenTryMatchPunct (parserCtx* ctx, punctTag punct) {
    if (tokenIsPunct(ctx, punct)) {
        tokenMatch(ctx);
        return true;

    } else
        return false;
}

void tokenMatchToken (parserCtx* ctx, tokenTag Match) {
    if (ctx->lexer->token == Match)
        tokenMatch(ctx);

    else {
        errorExpected(ctx, tokenTagGetStr(Match));
        tokenSkipMaybe(ctx);
    }
}

int tokenMatchInt (parserCtx* ctx) {
    int ret = atoi(ctx->lexer->buffer);

    tokenMatchToken(ctx, tokenInt);

    return ret;
}

char* tokenMatchIdent (parserCtx* ctx) {
    char* Old = strdup(ctx->lexer->buffer);

    tokenMatchToken(ctx, tokenIdent);

    return Old;
}

char* tokenMatchStr (parserCtx* ctx) {
    char* str = calloc(ctx->lexer->length, sizeof(char));

    if (tokenIsString(ctx)) {
        /*Iterate through the string excluding the first and last
          characters - the quotes*/
        for (int i = 1, length = 0; i+2 < ctx->lexer->length; i++) {
            /*Escape sequence*/
            if (ctx->lexer->buffer[i] == '\\') {
                i++;

                if (   ctx->lexer->buffer[i] == 'n' || ctx->lexer->buffer[i] == 'r'
                    || ctx->lexer->buffer[i] == 't' || ctx->lexer->buffer[i] == '\\'
                    || ctx->lexer->buffer[i] == '\'' || ctx->lexer->buffer[i] == '"') {
                    str[length++] = '\\';
                    str[length++] = ctx->lexer->buffer[i];

                /*An actual linebreak mid string? Escaped, ignore it*/
                } else if (   ctx->lexer->buffer[i] == '\n'
                         || ctx->lexer->buffer[i] == '\r')
                    i++;

                /*Unrecognised escape: ignore*/
                else
                    i++;

            } else
                str[length++] = ctx->lexer->buffer[i];
        }

        tokenMatch(ctx);

    } else
        errorExpected(ctx, "string");

    return str;
}
