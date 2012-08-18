#include <stdio.h>
#include <stdlib.h>
#include <string.h> //for strcmp
#include <stdint.h>
#include <stdbool.h>
#include "token.h"
#include "token_API.h"
#include "scanner.h"
#include "debug.h"
#include "lexer.h"

/**************************** STATIC HELPER FUNCTION PROTOTYPES **************************************/

static TOKEN lex(void);  //just gets a token from the input stream
static bool is_white_space(char c);
static void skip_whitespace_and_comments(void);
static void skip_single_line_comment(void);
static void skip_block_comments(void);
static bool block_comment_open_detected(void);
static bool block_comment_close_detected(void);
static bool single_line_comment_detected(void);
static TOKEN make_identifier_token(void);
static TOKEN make_number_token(void);
static TOKEN make_string_token(void);
static TOKEN make_special_token(void);
static void update_line_number(char c);
static int is_keyword_string(char* string);
static void get_identifier_string(char* buffer);
static void get_string_literal(char* buffer);
static uint64_t parse_number(void);

static bool is_delimiter_character(char c, DelimiterType* whichDelim);
static bool is_single_character_operator(char* buffer, OperatorType* whichOp);
static bool is_double_character_operator(char* buffer, OperatorType* whichOp);

/**************************** END OF STATIC HELPER FUNCTION PROTOTYPES *******************************/

uint64_t source_code_line_number = 1; //source files start with line 1

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
    "default",      
    "return",       
    "goto",         
    "extern",       
};                  

#define MAX_OPERATOR_LENGTH 3 //up to 2 characters for each operator plus the null terminal
static char operators[NUM_OPERATOR_TYPES+1][MAX_OPERATOR_LENGTH] = 
{
    "+",    // ADDITION
    "-",    // SUBTRACTION,            
    "",     // MULTIPLICATION,         //empty so the lexer will recognize the token as a STAR first
    "/",    // DIVISION,               
    "%",    // MODULAR_DIVISION,       
    "!",    // BOOLEAN_NOT,            
    "~",    // BITWISE_NOT,            
    "",     // BITWISE_AND,          //empty so the lexer will recognize the token as an AMPERSAND first  
    "|",    // BITWISE_OR,             
    "^",    // BITWISE_XOR,            
    "",     // REFERENCE,           //empty so the lexer will recognize the token as an AMPERSAND first
    "",     // DEREFERENCE,            //empty so the lexer will recognize the token as a STAR first
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
    "->",   //ARROW  (same as a dereference followed by a dot operator)
    "*",    //STAR 
    "&",    //AMPERSAND

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

    if(((int)(c = peek_char())) != EOF)
    {
        character_class = get_char_class(c);

        if(ALPHA == character_class)
        {
            token = make_identifier_token();
        }
        else if(NUMERIC == character_class)
        {
            token = make_number_token();
        }
        else if(SPECIAL == character_class)
        {
            token = make_special_token();
        }
        else
        {
            if('\"' == c)
            {
                token = make_string_token();
            }
            else if('\'' == c)
            {
                token = make_string_token();
            }
            else
            {
                fprintf(stderr, "Unkown character type '%c' encountered on line %lu\n", c, get_source_code_line_number());
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

static TOKEN lookahead = NULL;
static TOKEN current_token = NULL;

TOKEN get_token(void)
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

TOKEN peek_token(void)
{
    //printf("\nPEEKTOK\n\n");
    if((lookahead == current_token) || (lookahead == NULL))
    {
        lookahead = lex();
    }
    return lookahead;
}

//gets the line of source that the compiler is currently analyzing
uint64_t get_source_code_line_number(void)
{
    return source_code_line_number;
}

//updates the line number when a newline is encountered in the source code
static void update_line_number(char c)
{
    if(c == '\n')
    {
        source_code_line_number++;
    }
}


/*************** END OF LEXER ************************************************************************/

/************************** COMMENT PROCESSING FUNCTIONS *********************************************/

static void skip_whitespace_and_comments(void)
{
    char c = peek_char();
    
    while((((int)(c = peek_char())) != EOF) && (is_white_space(c) || single_line_comment_detected() || block_comment_open_detected()))
    {
        if(single_line_comment_detected())
        {
            skip_single_line_comment();
        }
        else if(block_comment_open_detected())
        {
            skip_block_comments();
        }
        else //is_white_space(c)
        {
            update_line_number(c);
            discard_char();
        }
    }
}

static bool single_line_comment_detected(void)
{
    bool result = false;

    if((peek_char() == '/') && (peek_2_char() == '/'))
    {
        result = true;
    }
    return result;
}

static bool block_comment_open_detected(void)
{
    bool result = false;
    if((peek_char() == '/') && (peek_2_char() == '*'))
    {
        result = true;
    }
    return result;
}

static bool block_comment_close_detected(void)
{
    bool result = false;
    if((peek_char() == '*') && (peek_2_char() == '/'))
    {
        result = true;
    }
    return result;
}

static void skip_single_line_comment(void)
{
    char c = peek_char();

    while(((c = peek_char()) != '\n') && ((int)c != EOF))
    {
       discard_char(); //get rid of everything in the comment
    }
    update_line_number(c);
    discard_char(); //discard the newline character
}

static void skip_block_comments(void)
{
    char c;
    uint64_t block_comment_starting_line = get_source_code_line_number();

    discard_char(); //discard the '/'
    discard_char(); //discard the '*'

    while(((int)(c = peek_char()) != EOF) && !block_comment_close_detected())
    {
        discard_char();
        update_line_number(c);
    }

    if(c == EOF)
    {
        fprintf(stderr, "The comment starting on line %lu needs a terminating comment symbol\n", block_comment_starting_line);
        exit(-1);
    }
    discard_char(); //discard the '*'
    discard_char(); //discard the '/'
}

static bool is_white_space(char c)
{
    bool result = false;
    if((c == '\t') || (c == '\n') || (c == ' '))
    {
        result = true;
    }
    return result;
}

/************************** END OF COMMENT PROCESSING FUNCTIONS **************************************/

/************************** IDENTIFIER PROCESSING FUNCTIONS ******************************************/

//makes an identifier token from the input stream
static TOKEN make_identifier_token(void)
{
    char buffer[MAX_TOKEN_STRING_LENGTH];
    TOKEN tok = make_token();

    //assume that the token is an identifier, then check if it is a reserved word
    setTokenType(tok, IDENTIFIER_TOKEN);
    setDataType(tok, STRING_TYPE);

    get_identifier_string(buffer);
    setStringVal(tok, buffer);

    int whichKeyword = is_keyword_string(getStringVal(tok));
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
static int is_keyword_string(char* string)
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
    char c = peek_char();
    char cclass = get_char_class(c);
    char prev_char;

    while( ((cclass == ALPHA) || (cclass == NUMERIC)) && (EOF != peek_char()) )
    {
        if(i < MAX_TOKEN_STRING_LENGTH)
        {
            buffer[i++] = c;
        }
        prev_char = get_char();
        c = peek_char();
        cclass = get_char_class(c);
    }
    buffer[i] = 0; //terminate the string
}

/************************** END OF IDENTIFIER PROCESSING FUNCTIONS ***********************************/

/************************** NUMBER PROCESSING FUNCTIONS **********************************************/

//makes number token from the input stream
static TOKEN make_number_token(void)
{
    TOKEN tok = make_token();
    setTokenType(tok, NUMBER_TOKEN); 
    uint64_t number = parse_number();
    setIntegerValue(tok, number);
    return tok;
}

static uint64_t parse_number(void)
{
    uint64_t result = 0;
    int i = 0;
    while(get_char_class(peek_char()) == NUMERIC)
    {
        i = get_char() - '0';
        result = result*10 +i;
        if(result > UINT32_MAX)
        {
            printf("\nNUMERIC CONSTANT ON LINE %lu BIGGER THAN MAX 32-BIT INTEGER VALUE\n", get_source_code_line_number());
            printf("CAPPING VALUE OF THE CONSTANT TO UINT32_MAX\n");
            result = UINT32_MAX;
            while(get_char_class(peek_char()) == NUMERIC)
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

//makes string token from the input stream
static TOKEN make_string_token(void)
{
    TOKEN tok = make_token();
    char buffer[MAX_TOKEN_STRING_LENGTH];
    setTokenType(tok, STRING_LITERAL);
    setDataType(tok, STRING_TYPE);
    get_string_literal(buffer);
    setStringVal(tok, buffer);
    return tok;
}

static void get_string_literal(char* buffer)
{
    int i = 0;
    discard_char(); //consume the opening quotation mark
    char c = peek_char();
    char cclass = get_char_class(c);
    char prev_char;

    while( (c != '\"')  && (EOF != peek_char()) )
    {
        if((peek_char() == '\\') && ((peek_2_char() == '\"') || (peek_2_char() == '\\')))
        {
            buffer[i++] = peek_2_char();
            discard_char();
            prev_char = get_char();
            c = peek_char();
            cclass = get_char_class(c);
            continue;
        }
        if(i < MAX_TOKEN_STRING_LENGTH)
        {
            buffer[i++] = c;
        }
        prev_char = get_char();
        c = peek_char();
        cclass = get_char_class(c);
    }
    buffer[i] = 0; //terminate the string
    discard_char(); //consume the closing quotation mark
}

/************************** END OF STRING PROCESSING FUNCTIONS ***************************************/

/************************** SPECIAL TOKEN PROCESSING FUNCTIONS ***************************************/

//make special token from the input stream
//handles all the delimiters and operators (both single and double character operators)
static TOKEN make_special_token(void)
{
    TOKEN tok = make_token();
    char buffer[MAX_OPERATOR_LENGTH];
    DelimiterType whichDelimiter;

    buffer[0] = peek_char();
    buffer[1] = peek_2_char();
    buffer[2] = 0; //terminate the string

    if(true == is_delimiter_character(buffer[0], &whichDelimiter))
    {
        buffer[1] = 0; //terminate the delimiter after 1 character
        setTokenType(tok, DELIMITER_TOKEN); 
        setWhichVal(tok, (int)whichDelimiter);
        discard_char();
    }
    else
    {
        OperatorType whichOp;
        if(true == is_double_character_operator(buffer, &whichOp))
        {
            setTokenType(tok, OPERATOR_TOKEN); 
            setWhichVal(tok, (int)whichOp);
            discard_char();
            discard_char(); //discard the characters to move to the next token
        }
        else 
        {
            buffer[1] = 0; //terminate the delimiter after 1 character
            if(true == is_single_character_operator(buffer, &whichOp))
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
    return tok;
}

//answers the question whether it is a delimiter, and if so which one
//if it is not a delimiter, then the value stored in whichDelim is invalid
static bool is_delimiter_character(char c, DelimiterType* whichDelim)
{
    int i = 0;
    bool result = false;
    *whichDelim = 0;
    for(i = 0; i < NUM_DELIMITER_TYPES; ++i)
    {
        if(delimiters[i] == c)
        {
            result = true;
            *whichDelim = i;
        }
    }
    return result;
}

//FIXME: this single/double character operator detection scheme is a FILTHY HACK
//FIXME: consider using a hash-table dictionary for more efficient lookup of tokens

//answers the question whether it is an operator, and if so which one
//if it is not a operator, then the value stored in whichOp is invalid
static bool is_double_character_operator(char* buffer, OperatorType* whichOp)
{
    int i = 0;
    bool result = false;
    *whichOp = 0;

    for(i = EQUALS; i < NUM_OPERATOR_TYPES; ++i)
    {
        if(0 == strcmp(operators[i], buffer))
        {
            result = true;
            *whichOp = i;
        }
    }
    return result;
}

//answers the question whether it is an operator, and if so which one
//if it is not a operator, then the value stored in whichOp is invalid
static bool is_single_character_operator(char* buffer, OperatorType* whichOp)
{
    int i = 0;
    bool result = false;
    *whichOp = 0;

    for(i = ADDITION; i < NUM_OPERATOR_TYPES; ++i)
    {
        if(0 == strcmp(operators[i], buffer))
        {
            result = true;
            *whichOp = i;
        }
    }
    return result;
}

/************************** END OF SPECIAL TOKEN PROCESSING FUNCTIONS ********************************/
