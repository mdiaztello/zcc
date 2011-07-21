#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"
#include "global_defs.h"

struct token
{
    enum token_type tokenType;
    enum data_type dataType;
    //struct symtbr * symtype;
    //struct symtbr * symentry;
    struct token* operands;
    struct token* link;
    union
    {
        char tokenstring[16];
        int which;
        long integer_number;
        double floating_number;
    } tokenvalue;

};

TOKEN makeToken(void)
{
    TOKEN t = (TOKEN)malloc(sizeof(struct token));
    if(t == NULL)
    {
        printf("Token allocation failed\n");
    }
    return t;
}

void printToken(TOKEN t)
{
    printTokenType(t);
}

void printTokenType(TOKEN t)
{
    printf("TOKEN TYPE:\t");
    switch(t->tokenType)
    {
        case KEYWORD_TOKEN:
            printf("KEYWORD_TOKEN\n");
            break;
        case DELIMITER_TOKEN:
            printf("DELIMITER_TOKEN\n");
            break;
        case STRING_LITERAL:
            printf("STRING_LITERAL\n");
            break;
        case CHARACTER_LITERAL:
            printf("CHARACTER_LITERAL\n");
            break;
        case IDENTIFIER_TOKEN:
            printf("IDENTIFIER_TOKEN\n");
            break;
        case OPERATOR_TOKEN:
            printf("OPERATOR_TOKEN\n");
            break;
        case NUMBER_TOKEN:
            printf("NUMBER_TOKEN\n");
            break;
        default: 
            printf("unknown");
            break;
    }
}
