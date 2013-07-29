#include "../std/std.h"

#include "vector.h"

typedef enum {
    modeDefault,
    modeNoAssemble,
    modeNoLink,
    modeVersion,
    modeHelp
} configMode;

typedef struct {
    bool fail;
    configMode mode;

    vector/*<char*>*/ inputs, intermediates;
    char* output;
} config;

config configCreate ();
void configDestroy (config conf);

void optionsParse (config* conf, int argc, char** argv);