#include <stdio.h>
#include "token.h"
#include "scanner.h"
#include "global_defs.h"
#include "debug.h"

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

unsigned long source_code_line_number = 0;

TOKEN lex(void)
{
    TOKEN token = makeToken();
    int c;
    int character_class;

    skip_whitespace_and_comments();

    if((c = peekchar()) != EOF)
    {
        character_class = CHARCLASS[c];

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

}

TOKEN make_identifier(TOKEN tok)
{

    return tok;
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

void updateLineNumber(char c)
{
    if(c == '\n')
    {
        source_code_line_number++;
    }
}
