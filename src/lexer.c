#include "../inc/lexer.h"

#include "../std/std.h"

#include "../inc/stream.h"

#include "stdlib.h"
#include "ctype.h"

lexerCtx* lexerInit (const char* File) {
    lexerCtx* ctx = malloc(sizeof(lexerCtx));
    ctx->stream = streamInit(File);
    ctx->token = tokenUndefined;
    ctx->bufferSize = 64;
    ctx->buffer = malloc(sizeof(char)*ctx->bufferSize);
    lexerNext(ctx);
    return ctx;
}

void lexerEnd (lexerCtx* ctx) {
    streamEnd(ctx->stream);
    free(ctx->buffer);
    free(ctx);
}

/*Eat as many insignificants (comments, whitespace etc) as possible*/
static void lexerSkipInsignificants (lexerCtx* ctx) {
    while (ctx->stream->current != 0) {
        /*Whitespace*/
        if (   ctx->stream->current == ' '
            || ctx->stream->current == '\t'
            || ctx->stream->current == '\n'
            || ctx->stream->current == '\r')
            streamNext(ctx->stream);

        /*C preprocessor is treated as a comment*/
        else if (ctx->stream->current == '#') {
            /*Eat until a new line*/
            while (   ctx->stream->current != '\n'
                   && ctx->stream->current != 0)
                streamNext(ctx->stream);

            streamNext(ctx->stream);

        /*Comment?*/
        } else if (ctx->stream->current == '/') {
            streamNext(ctx->stream);

            /*C comment*/
            if (ctx->stream->current == '*') {
                streamNext(ctx->stream);

                do {
                    while (   ctx->stream->current != '*'
                           && ctx->stream->current != 0)
                        streamNext(ctx->stream);

                    if (ctx->stream->current == 0)
                        break;

                    streamNext(ctx->stream);
                } while (   ctx->stream->current != '/'
                         && ctx->stream->current != 0);

                streamNext(ctx->stream);

            /*C++ Comment*/
            } else if (ctx->stream->current == '/') {
                streamNext(ctx->stream);

                while (   ctx->stream->current != '\n'
                       && ctx->stream->current != '\r')
                    streamNext(ctx->stream);

            /*Fuck, we just ate an important character. Backtrack!*/
            } else {
                streamPrev(ctx->stream);
                break;
            }

        /*Not insignificant, leave*/
        } else
            break;

    }
}

static void lexerEat (lexerCtx* ctx, char c) {
    /*Buffer full? Double the size*/
    if (ctx->length+1 == ctx->bufferSize)
        realloc(ctx->buffer, ctx->bufferSize *= 2);

    ctx->buffer[ctx->length++] = c;
}

void lexerNext (lexerCtx* ctx) {
    if (ctx->token == tokenEOF)
        return;

    lexerSkipInsignificants(ctx);

    ctx->length = 0;

    /*End of stream. Do not increment pos, as we could segfault.
      Continued calls to this function will prepare more EOF tokens.*/
    if (ctx->stream->current == 0)
        ctx->token = tokenEOF;

    /* Idents, a letter followed by any number (zero?) of alphanumerics*/
    else if (   isalpha(ctx->stream->current)
             || ctx->stream->current == '_') {
        ctx->token = tokenIdent;
        lexerEat(ctx, streamNext(ctx->stream));

        while (   isalnum(ctx->stream->current)
               || ctx->stream->current == '_')
            lexerEat(ctx, streamNext(ctx->stream));

    }
    /*Number*/
    else if (isdigit(ctx->stream->current)) {
        ctx->token = tokenInt;

        while (isdigit(ctx->stream->current))
            lexerEat(ctx, streamNext(ctx->stream));

    /*String*/
    } else if (ctx->stream->current == '"') {
        ctx->token = tokenStr;
        lexerEat(ctx, streamNext(ctx->stream));

        while (ctx->stream->current != '"') {
            if (ctx->stream->current == '\\')
                lexerEat(ctx, streamNext(ctx->stream));

            lexerEat(ctx, streamNext(ctx->stream));
        }

        lexerEat(ctx, streamNext(ctx->stream));

    /*Character*/
    } else if (ctx->stream->current == '\'') {
        ctx->token = tokenChar;
        lexerEat(ctx, streamNext(ctx->stream));

        while (!ctx->stream->current == '\'') {
            if (ctx->stream->current == '\\')
                lexerEat(ctx, streamNext(ctx->stream));

            lexerEat(ctx, streamNext(ctx->stream));
        }

        lexerEat(ctx, streamNext(ctx->stream));

    /*Other symbol. Operators come into this category*/
    } else {
        ctx->token = tokenOther;
        lexerEat(ctx, streamNext(ctx->stream));

        /*If it is a double char operator like != or ->, combine them*/

        /* == !=  += -= *= /= %=  &= |= ^= */
        if (   (   (   ctx->buffer[0] == '=' || ctx->buffer[0] == '!'
                    || ctx->buffer[0] == '+' || ctx->buffer[0] == '-'
                    || ctx->buffer[0] == '*' || ctx->buffer[0] == '/'
                    || ctx->buffer[0] == '%'
                    || ctx->buffer[0] == '&' || ctx->buffer[0] == '|'
                    || ctx->buffer[0] == '^')
                && ctx->stream->current == '=')
        /* -> */
            || (ctx->buffer[0] == '-'
                && (ctx->stream->current == '>' || ctx->stream->current == '-'))
        /* && || ++ --*/
            || (   (   ctx->buffer[0] == '&' || ctx->buffer[0] == '|'
                    || ctx->buffer[0] == '+' || ctx->buffer[0] == '-')
                && ctx->stream->current == ctx->buffer[0])
        /* >= <= */
            || ((ctx->buffer[0] == '>' || ctx->buffer[0] == '<')
                && ctx->stream->current == '='))
            lexerEat(ctx, streamNext(ctx->stream));

        /*Possible triple char operator*/

        /* >> << */
        if (   (ctx->buffer[0] == '>' || ctx->buffer[0] == '<')
            && (ctx->stream->current == ctx->buffer[0])) {
            lexerEat(ctx, streamNext(ctx->stream));

            /* >>= <<=*/
            if (ctx->stream->current == '=')
                lexerEat(ctx, streamNext(ctx->stream));
        }
    }

    lexerEat(ctx, 0);

    //printf("token(%d:%d): '%s'.\n", ctx->stream->line, ctx->stream->lineChar, ctx->buffer);
}
