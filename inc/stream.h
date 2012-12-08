#pragma once

#include "stdio.h"

/**
 * Stream context
 */
typedef struct {
    char* filename;
    FILE* file;

    char current;
    int line;
    int lineChar;
} streamCtx;

streamCtx* streamInit (char* File);
void streamEnd (streamCtx* ctx);

/**
 * Load the next character, return the old character
 */
char streamNext (streamCtx* ctx);

/**
 * Backtrack a single character, return the old character (that is now next)
 */
char streamPrev (streamCtx* ctx);
