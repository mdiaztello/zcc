#include "lexer.h"
#include <stdio.h>



//handles errors where tokens (whether valid or invalid)
//occur between a functions declaration line and its body
void compound_statement_error_handler(void)
{
    printf("ERROR: Line %5lu\tIllegal token between function prototype and function body.\n", getSourceLine());
}
