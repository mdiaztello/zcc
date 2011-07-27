#include <stdio.h>
#include "token.h"
#include "scanner.h"
#include "global_defs.h"
#include "debug.h"
#include <stdlib.h>

static void skip_whitespace_and_comments(void);
static TOKEN make_identifier(TOKEN tok);
static TOKEN make_number(TOKEN tok);
static TOKEN make_string(TOKEN tok);
static TOKEN make_special(TOKEN tok);
BOOLEAN isWhiteSpace(char c);
BOOLEAN detectBlockCommentClose(void);
BOOLEAN detectBlockCommentOpen(void);
BOOLEAN detectedSingleLineComment(void);
static void skip_single_line_comment(void);
void skip_block_comments(void);
static void updateLineNumber(char c);

unsigned char keywords[NUM_KEYWORD_TYPES][MAX_KEYWORD_LENGTH] =
{
    "static",
    "const",
    "unsigned", 
    "signed", 
    "char", 
    "short", 
    "int", 
    "long",
    "float",
    "double",
    "struct",
    "union",
    "enum",
    "void",
    "typedef",
    "if",
    "else",
    "do",
    "while",
    "for",
    "continue",
    "switch",
    "case",
    "break",
    "return",
    "goto",

};

unsigned long source_code_line_number = 0;

TOKEN lex(void)
{
    TOKEN token = makeToken();
    int c;
    int character_class;

    skip_whitespace_and_comments();

    if((c = peekchar()) != EOF)
    {
        character_class = get_char_class(c);

        if(ALPHA == character_class)
        {
            make_identifier(token);
        }
        else if(NUMERIC == character_class)
        {
            make_number(token);
        }
        else if('\'' == c)
        {
            make_string(token);
        }
        else
        {
            make_special(token);
        }
    }
    else
    {
        EOFFLG = 1;
    }

    if(DEBUG_GET_TOKEN != 0)
    {
        printtoken(token);
    }
    return token;
}

void skip_whitespace_and_comments(void)
{
    char c = peekchar();
    
    int i = 0;
    while(((c = peekchar()) != EOF) && (isWhiteSpace(c) || detectedSingleLineComment() || detectBlockCommentOpen()))
    {
        if(detectedSingleLineComment())
        {
            skip_single_line_comment();
            printf("finished\n");
        }
        else if(detectBlockCommentOpen())
        {
            skip_block_comments();
        }
        else //isWhiteSpace(c)
        {
            updateLineNumber(c);
            c = getchar();
        }
    }
}

BOOLEAN detectedSingleLineComment(void)
{
    BOOLEAN result = FALSE;

    if((peekchar() == '/') && (peek2char() == '/'))
    {
        result = TRUE;
    }
    return result;
}

BOOLEAN detectBlockCommentOpen(void)
{
    BOOLEAN result = FALSE;
    if((peekchar() == '/') && (peek2char() == '*'))
    {
        result = TRUE;
    }
    return result;
}

BOOLEAN detectBlockCommentClose(void)
{
    BOOLEAN result = FALSE;
    if((peekchar() == '*') && (peek2char() == '/'))
    {
        result = TRUE;
    }
    return result;
}

void skip_single_line_comment(void)
{
    char c = peekchar();

    while(((c = peekchar()) != '\n') && (c != EOF))
    {
       getchar();
    }
    updateLineNumber(c);
    getchar();
    dprint("we are on line %d\n", source_code_line_number);
}


void skip_block_comments(void)
{
    char c;
    getchar(); //discard the '/'
    getchar(); //discard the '*'

    unsigned long block_comment_starting_line = source_code_line_number;

    while(((c = peekchar()) != EOF) && !detectBlockCommentClose())
    {
        c = getchar();
        updateLineNumber(c);
    }

    if(c == EOF)
    {
        fprintf(stderr, "The comment starting on line %lu needs a terminating comment symbol\n", block_comment_starting_line);
        exit(-1);
    }
    getchar(); //discard the '*'
    getchar(); //discard the '/'

}

TOKEN make_identifier(TOKEN tok)
{
    char buffer[MAX_TOKEN_LENGTH];

    setTokenType(tok, IDENTIFIER_TOKEN);
    setDataType(tok, STRING_TYPE);

    //assume that the token is an identifier, then check if it is a reserved word
    get_identifier_string(buffer);
    
    setStringVal(tok, buffer);

    return tok;
}

static void get_identifier_string(char* buffer)
{
    int i = 0;
    char c = peekchar();
    char cclass = get_char_class(c);
    char prev_char;

    while( ((cclass == ALPHA) || (cclass == NUMERIC) || (c == '_')) && (EOF != peekchar()) )
    {
        if(i < MAX_TOKEN_LENGTH)
        {
            buffer[i++] = c;
        }
        prev_char = getchar();
        c = peekchar();
        cclass = get_char_class(c);
    }
    buffer[i] = 0; //terminate the string
}




TOKEN make_number(TOKEN tok)
{

    return tok;
}



TOKEN make_string(TOKEN tok)
{

    return tok;
}



TOKEN make_special(TOKEN tok)
{

    return tok;
}


BOOLEAN isWhiteSpace(char c)
{
    BOOLEAN result = FALSE;
    if((c == '\t') || (c == '\n')
            || (c == ' '))
    {
        result = TRUE;
    }

    return result;
}

static void updateLineNumber(char c)
{
    if(c == '\n')
    {
        source_code_line_number++;
    }
}
