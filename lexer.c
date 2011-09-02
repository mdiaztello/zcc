#include <stdio.h>
#include <stdlib.h>
#include <string.h> //for strcmp
#include "token.h"
#include "token_API.h"
#include "scanner.h"
#include "global_defs.h"
#include "debug.h"

/**************************** STATIC HELPER FUNCTION PROTOTYPES **************************************/

static void skip_whitespace_and_comments(void);
static void skip_single_line_comment(void);
static void skip_block_comments(void);
static BOOLEAN isWhiteSpace(char c);
static BOOLEAN detectBlockCommentClose(void);
static BOOLEAN detectBlockCommentOpen(void);
static BOOLEAN detectedSingleLineComment(void);
static void make_identifier(TOKEN tok);
static void make_number(TOKEN tok);
static void make_string(TOKEN tok);
static void make_special(TOKEN tok);
static void updateLineNumber(char c);
static int isKeywordString(char* string);
static void get_identifier_string(char* buffer);
static void get_string_literal(char* buffer);
static long long parse_number(void);

static BOOLEAN isDelimiterCharacter(char c, DelimiterType* whichDelim);
static BOOLEAN isDoubleCharacterOperator(char* buffer, OperatorType* whichOp);
static BOOLEAN isSingleCharacterOperator(char* buffer, OperatorType* whichOp);

/**************************** END OF STATIC HELPER FUNCTION PROTOTYPES *******************************/

unsigned long source_code_line_number = 1; //source files start with line 1
static char keywords[NUM_KEYWORD_TYPES][MAX_KEYWORD_LENGTH] =
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
    "extern",
};

#define MAX_OPERATOR_LENGTH 3 //up to 2 characters for each operator plus the null terminal
static char operators[NUM_OPERATOR_TYPES+1][MAX_OPERATOR_LENGTH] = 
{
    "+",    // ADDITION
    "-",    // SUBTRACTION,            
    "*",    // MULTIPLICATION,         
    "/",    // DIVISION,               
    "%",    // MODULAR_DIVISION,       
    "!",    // BOOLEAN_NOT,            
    "~",    // BITWISE_NOT,            
    "&",    // BITWISE_AND,            
    "|",    // BITWISE_OR,             
    "^",    // BITWISE_XOR,            
    //"&",  // REFERENCE,              
    //"*",  // DEREFERENCE,            
    ".",    // DOT,                    
    "=",    // ASSIGNMENT,             
    ">",    // GREATER_THAN,           
    "<",    // LESS_THAN,              

    "==",   // EQUALS,                 
    "!=",   // NOT_EQUALS,             
    ">=",   // GREATER_THAN_OR_EQUAL,  
    "<=",   // LESS_THAN_OR_EQUAL,     
    "||",   // BOOLEAN_OR,             
    "&&",   // BOOLEAN_AND,            
    "<<",   //shift left      //NOTE: I AM EXPLICITLY DISSALLOWING "<<="
    ">>",   //shift right     //NOTE: I AM EXPLICITLY DISSALLOWING ">>="
    "++",   //increment  (post/pre)
    "--",   //decrement (post/pre)
    "+=",   //PLUS_EQUAL
    "-=",   //MINUS_EQUAL
    "*=",   //MULTIPLY_EQUAL
    "/=",   //DIVIDE_EQUAL
    "%=",   //MOD_EQUAL
    "&=",   //BITWISE_AND_EQUAL
    "|=",   //BITWISE_OR_EQUAL
    "->"    //ARROW  (same as a dereference followed by a dot operator)
};

//TODO: consider using hash tables for all of this stuff since it may be faster/easier
static char delimiters[NUM_DELIMITER_TYPES+1] = ",;:()[]{}"; 

/*************** LEXER *******************************************************************************/

TOKEN lex(void)
{
    TOKEN token = NULL;
    char c;
    int character_class;

    skip_whitespace_and_comments();

    if(((int)(c = peekchar())) != EOF)
    {
        token = makeToken();
        character_class = get_char_class(c);

        if(ALPHA == character_class)
        {
            make_identifier(token);
        }
        else if(NUMERIC == character_class)
        {
            make_number(token);
        }
        else if(SPECIAL == character_class)
        {
            make_special(token);
        }
        else
        {
            if('\"' == c)
            {
                make_string(token);
            }
            else if('\'' == c)
            {
                make_string(token);
            }
            else
            {
                printf("what the fuck just happened?\n");
            }
        }
    }
    else
    {
        EOFFLG = 1;
        source_code_line_number--; //if we hit the end of the file, we've counted one line too many
    }

    if(DEBUG_GET_TOKEN != 0)
    {
        printToken(token);
    }
    return token;
}

TOKEN lookahead = NULL;
TOKEN current_token = NULL;

TOKEN gettok(void)
{
    //printf("\nGETTOK\n\n");
    if((current_token != lookahead) && (lookahead != NULL))
    {
        current_token = lookahead;
    }
    else
    {
        current_token = lex();
        lookahead = NULL;
    }
    return current_token;
}

TOKEN peektok(void)
{
    //printf("\nPEEKTOK\n\n");
    if((lookahead == current_token) || (lookahead == NULL))
    {
        lookahead = lex();
    }
    return lookahead;
}


/*************** END OF LEXER ************************************************************************/

/************************** COMMENT PROCESSING FUNCTIONS *********************************************/

static void skip_whitespace_and_comments(void)
{
    char c = peekchar();
    
    while((((int)(c = peekchar())) != EOF) && (isWhiteSpace(c) || detectedSingleLineComment() || detectBlockCommentOpen()))
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
            discard_char();
        }
    }
}

static BOOLEAN detectedSingleLineComment(void)
{
    BOOLEAN result = FALSE;

    if((peekchar() == '/') && (peek2char() == '/'))
    {
        result = TRUE;
    }
    return result;
}

static BOOLEAN detectBlockCommentOpen(void)
{
    BOOLEAN result = FALSE;
    if((peekchar() == '/') && (peek2char() == '*'))
    {
        result = TRUE;
    }
    return result;
}

static BOOLEAN detectBlockCommentClose(void)
{
    BOOLEAN result = FALSE;
    if((peekchar() == '*') && (peek2char() == '/'))
    {
        result = TRUE;
    }
    return result;
}

static void skip_single_line_comment(void)
{
    char c = peekchar();

    while(((c = peekchar()) != '\n') && ((int)c != EOF))
    {
       discard_char(); //get rid of everything in the comment
    }
    updateLineNumber(c);
    discard_char(); //getchar(); //discard the newline character
}

static void skip_block_comments(void)
{
    char c;
    unsigned long block_comment_starting_line = source_code_line_number;

    discard_char(); //getchar(); //discard the '/'
    discard_char(); //getchar(); //discard the '*'

    while(((int)(c = peekchar()) != EOF) && !detectBlockCommentClose())
    {
        discard_char();
        updateLineNumber(c);
    }

    if(c == EOF)
    {
        fprintf(stderr, "The comment starting on line %lu needs a terminating comment symbol\n", block_comment_starting_line);
        exit(-1);
    }
    discard_char(); //getchar(); //discard the '*'
    discard_char(); //getchar(); //discard the '/'
}

static BOOLEAN isWhiteSpace(char c)
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

/************************** END OF COMMENT PROCESSING FUNCTIONS **************************************/

/************************** IDENTIFIER PROCESSING FUNCTIONS ******************************************/

//makes an identifier token from a pre-allocated token object
static void make_identifier(TOKEN tok)
{
    char buffer[MAX_TOKEN_STRING_LENGTH];

    //assume that the token is an identifier, then check if it is a reserved word
    setTokenType(tok, IDENTIFIER_TOKEN);
    setDataType(tok, STRING_TYPE);

    get_identifier_string(buffer);
    setStringVal(tok, buffer);

    int whichKeyword = isKeywordString(getStringVal(tok));
    if(-1 != whichKeyword)
    {
        setWhichVal(tok, whichKeyword);
        setTokenType(tok, KEYWORD_TOKEN);
    }
}

//returns which keyword the string represents
//or returns NOT_A_KEYWORD if it isn't in the
//dictionary of keywords
static int isKeywordString(char* string)
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

    while( ((cclass == ALPHA) || (cclass == NUMERIC)) && (EOF != peekchar()) )
    {
        if(i < MAX_TOKEN_STRING_LENGTH)
        {
            buffer[i++] = c;
        }
        prev_char = getchar();
        c = peekchar();
        cclass = get_char_class(c);
    }
    buffer[i] = 0; //terminate the string
}

/************************** END OF IDENTIFIER PROCESSING FUNCTIONS ***********************************/

/************************** NUMBER PROCESSING FUNCTIONS **********************************************/

//makes number token from pre-allocated token object
static void make_number(TOKEN tok)
{
    setTokenType(tok, NUMBER_TOKEN); 
    long long number = parse_number();
    setIntegerValue(tok, number);
}

#define MAX_UNSIGNED_32_BIT_INTEGER 4294967295U

static long long parse_number(void)
{
    long long result = 0;
    int i = 0;
    while(get_char_class(peekchar()) == NUMERIC)
    {
        i = getchar() - '0';
        result = result*10 +i;
        if(result > MAX_UNSIGNED_32_BIT_INTEGER)
        {
            printf("\nNUMERIC CONSTANT ON LINE %lu BIGGER THAN MAX 32-BIT INTEGER VALUE\n", source_code_line_number);
            printf("CAPPING VALUE OF THE CONSTANT TO MAX_UNSIGNED_32_BIT_INTEGER\n");
            result = MAX_UNSIGNED_32_BIT_INTEGER;
            while(get_char_class(peekchar()) == NUMERIC)
            {
                discard_char(); //discard the rest of the digits
            }
            break;
        }
    }
    return result;
}

/************************** END OF NUMBER PROCESSING FUNCTIONS ***************************************/

/************************** STRING PROCESSING FUNCTIONS **********************************************/

//makes string token from pre-allocated token object
static void make_string(TOKEN tok)
{
    char buffer[MAX_TOKEN_STRING_LENGTH];
    setTokenType(tok, STRING_LITERAL);
    setDataType(tok, STRING_TYPE);
    get_string_literal(buffer);
    setStringVal(tok, buffer);
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
        if(i < MAX_TOKEN_STRING_LENGTH)
        {
            buffer[i++] = c;
        }
        prev_char = getchar();
        c = peekchar();
        cclass = get_char_class(c);
    }
    buffer[i] = 0; //terminate the string
    discard_char(); //consume the closing quotation mark
}

/************************** END OF STRING PROCESSING FUNCTIONS ***************************************/

/************************** SPECIAL TOKEN PROCESSING FUNCTIONS ***************************************/

//make special token from pre-allocated token object
//handles all the delimiters and operators (both single and double character operators)
static void make_special(TOKEN tok)
{
    char buffer[MAX_OPERATOR_LENGTH];
    DelimiterType whichDelimiter;

    buffer[0] = peekchar();
    buffer[1] = peek2char();
    buffer[2] = 0; //terminate the string

    if(TRUE == isDelimiterCharacter(buffer[0], &whichDelimiter))
    {
        buffer[1] = 0; //terminate the delimiter after 1 character
        setTokenType(tok, DELIMITER_TOKEN); 
        setWhichVal(tok, (int)whichDelimiter);
        discard_char();
    }
    else
    {
        OperatorType whichOp;
        if(TRUE == isDoubleCharacterOperator(buffer, &whichOp))
        {
            setTokenType(tok, OPERATOR_TOKEN); 
            setWhichVal(tok, (int)whichOp);
            discard_char();
            discard_char(); //discard the characters to move to the next token
        }
        else 
        {
            buffer[1] = 0; //terminate the delimiter after 1 character
            if(TRUE == isSingleCharacterOperator(buffer, &whichOp))
            {
                setTokenType(tok, OPERATOR_TOKEN); 
                setWhichVal(tok, (int)whichOp);
                discard_char();
            }
            else
            {
                printf("This should never have happened!\n");
            }
        }
    }
}

//answers the question whether it is a delimiter, and if so which one
//if it is not a delimiter, then the value stored in whichDelim is invalid
static BOOLEAN isDelimiterCharacter(char c, DelimiterType* whichDelim)
{
    int i = 0;
    BOOLEAN result = FALSE;
    *whichDelim = 0;
    for(i = 0; i < NUM_DELIMITER_TYPES; ++i)
    {
        if(delimiters[i] == c)
        {
            result = TRUE;
            *whichDelim = i;
        }
    }
    return result;
}

//FIXME: this single/double character operator detection scheme is a FILTHY HACK
//FIXME: consider using a hash-table dictionary for more efficient lookup of tokens

//answers the question whether it is an operator, and if so which one
//if it is not a operator, then the value stored in whichOp is invalid
static BOOLEAN isDoubleCharacterOperator(char* buffer, OperatorType* whichOp)
{
    int i = 0;
    BOOLEAN result = FALSE;
    *whichOp = 0;

    for(i = EQUALS; i < NUM_OPERATOR_TYPES; ++i)
    {
        if(0 == strcmp(operators[i], buffer))
        {
            result = TRUE;
            *whichOp = i;
        }
    }
    return result;
}

//answers the question whether it is an operator, and if so which one
//if it is not a operator, then the value stored in whichOp is invalid
static BOOLEAN isSingleCharacterOperator(char* buffer, OperatorType* whichOp)
{
    int i = 0;
    BOOLEAN result = FALSE;
    *whichOp = 0;

    for(i = ADDITION; i < NUM_OPERATOR_TYPES; ++i)
    {
        if(0 == strcmp(operators[i], buffer))
        {
            result = TRUE;
            *whichOp = i;
        }
    }
    return result;
}

/************************** END OF SPECIAL TOKEN PROCESSING FUNCTIONS ********************************/
