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
        char tokenstring[MAX_TOKEN_LENGTH];
        int which;
        long integer_number;
        double floating_number;
    } tokenvalue;

};

#define stringval  tokenvalue.tokenstring
#define whichval tokenvalue.which

TOKEN makeToken(void)
{
    TOKEN t = (TOKEN)malloc(sizeof(struct token));
    if(t == NULL)
    {
        printf("Token allocation failed\n");
    }
    return t;
}

void setTokenType(TOKEN t, TokenType tType)
{
    t->tokenType = tType;
}

TokenType getTokenType(TOKEN t)
{
    return t->tokenType;
}

void setDataType(TOKEN t, DataType dType)
{
    t->dataType = dType;
}

DataType getDataType(TOKEN t)
{
    return t->dataType;
}

void setStringVal(TOKEN t, char* string)
{
    int i = 0;

    for(i = 0; (i < MAX_TOKEN_LENGTH) && (string[i] != 0); i++)
    {
        t->stringval[i] = string[i];
    }
    t->stringval[i] = 0; //terminate the string FIXME: there may be an off-by-one buffer overflow here...
}

char* getStringVal(TOKEN t)
{
    return t->stringval;
}

void setWhichVal(TOKEN t, int which)
{
    t->whichval = which;
}

int getWhichVal(TOKEN t)
{
    return t->whichval;
}

void printToken(TOKEN t)
{
    printf("\n");
    printTokenType(t);
    switch(getTokenType(t))
    {
        case KEYWORD_TOKEN:
            printKeywordType(t);
            break;
        case IDENTIFIER_TOKEN:
            printIdentifier(t);
            break;
        case STRING_LITERAL:
            printf("The string is \"%s\"\n", getStringVal(t));
            break;
        case DELIMITER_TOKEN:
            printf("The DELIMITER we found is a ");
            printDelimiterType(t);
            break;
        default:
            printf("I don't know what kind of token this is...\n");
            break;
    }
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

void printKeywordType(TOKEN t)
{
    printf("This is a \"");
    switch(getWhichVal(t))
    {
        case STATIC:
            printf("static");
            break;
        case CONST:
            printf("const");
            break;
        case UNSIGNED:
            printf("unsigned");
            break;
        case SIGNED:
            printf("signed");
            break;
        case CHAR:
            printf("char");
            break;
        case SHORT:
            printf("short");
            break;
        case INT:
            printf("int");
            break;
        case LONG:
            printf("long");
            break;
        case FLOAT:
            printf("float");
            break;
        case DOUBLE:
            printf("double");
            break;
        case STRUCT:
            printf("struct");
            break;
        case UNION:
            printf("union");
            break;
        case ENUM:
            printf("enum");
            break;
        case VOID:
            printf("void");
            break;
        case TYPEDEF:
            printf("typedef");
            break;
        case IF:
            printf("if");
            break;
        case ELSE:
            printf("else");
            break;
        case DO:
            printf("do");
            break;
        case WHILE:
            printf("while");
            break;
        case FOR:
            printf("for");
            break;
        case CONTINUE:
            printf("continue");
            break;
        case SWITCH:
            printf("switch");
            break;
        case CASE:
            printf("case");
            break;
        case BREAK:
            printf("break");
            break;
        case RETURN:
            printf("return");
            break;
        case GOTO:
            printf("goto");
            break;

        default:
            printf("UNKNOWN");
            break;
    }
    printf("\" keyword token\n");
}

void printIdentifier(TOKEN t)
{
    printf("The identifier is \"%s\"\n", getStringVal(t));
}

void printDelimiterType(TOKEN t)
{
    switch(getWhichVal(t))
    {
        case COMMA:
            printf("COMMA\n");
            break;
        case SEMICOLON:
            printf("SEMICOLON\n");
            break;
        case COLON:
            printf("COLON\n");
            break;
        case OPEN_PAREN:
            printf("OPEN_PAREN\n");
            break;
        case CLOSE_PAREN:
            printf("CLOSE_PAREN\n");
            break;
        case OPEN_BRACKET:
            printf("OPEN_BRACKET\n");
            break;
        case CLOSE_BRACKET:
            printf("CLOSE_BRACKET\n");
            break;
        case OPEN_BRACE:
            printf("OPEN_BRACE\n");
            break;
        case CLOSE_BRACE:
            printf("CLOSE_BRACE\n");
            break;
        default:
            printf("UNKOWN DELIMITER\n");
            break;
    }
}
