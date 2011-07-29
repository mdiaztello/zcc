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
static int isKeyword(char* string);
static void get_identifier_string(char* buffer);
static void get_string_literal(char* buffer);

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
        else if('\"' == c)
        {
            make_string(token);
        }
        else if('\'' == c)
        {
            //make_string(token);
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

    //assume that the token is an identifier, then check if it is a reserved word
    setTokenType(tok, IDENTIFIER_TOKEN);
    setDataType(tok, STRING_TYPE);

    get_identifier_string(buffer);
    setStringVal(tok, buffer);

    int whichKeyword = isKeyword(getStringVal(tok));
    if(-1 != whichKeyword)
    {
        setWhichVal(tok, whichKeyword);
        setTokenType(tok, KEYWORD_TOKEN);
    }

    return tok;
}

//returns which keyword the string represents
//or returns NOT_A_KEYWORD if it isn't in the
//dictionary of keywords
static int isKeyword(char* string)
{
#define NOT_A_KEYWORD (-1)
    int i = 0;
    int whichKeyword = NOT_A_KEYWORD;

    for(i = 0; i < NUM_KEYWORD_TYPES; i++)
    {
        if(0 == strcmp(string, keywords[i]))
        {
            whichKeyword = i;
            break;
        }
    }
    return whichKeyword;
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
    /* There is vomit _EVERYWHERE_!! */

    return tok;
}



TOKEN make_string(TOKEN tok)
{
    char buffer[MAX_TOKEN_LENGTH];
    setTokenType(tok, STRING_LITERAL);
    setDataType(tok, STRING_TYPE);
    get_string_literal(buffer);
    setStringVal(tok, buffer);

    return tok;
}

static void get_string_literal(char* buffer)
{
    int i = 0;
    getchar(); //consume the opening quotation mark
    char c = peekchar();
    char cclass = get_char_class(c);
    char prev_char;

    while( (c != '\"')  && (EOF != peekchar()) )
    {
        if((peekchar() == '\\') && ((peek2char() == '\"') || (peek2char() == '\\')))
        {
            buffer[i++] = peek2char();
            getchar();
            prev_char = getchar();
            c = peekchar();
            cclass = get_char_class(c);
            continue;
        }
        if(i < MAX_TOKEN_LENGTH)
        {
            buffer[i++] = c;
        }
        prev_char = getchar();
        c = peekchar();
        cclass = get_char_class(c);
    }
    buffer[i] = 0; //terminate the string
    getchar(); //consume the closing quotation mark
}


TOKEN make_special(TOKEN tok)
{

    return tok;
}


BOOLEAN isWhiteSpace(char c)
{
    BOOLEAN result = FALSE;
    if((c == '\t') || (c == '\n') || (c == ' '))
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
