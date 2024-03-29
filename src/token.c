#include "token.h"
#include "token_API.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "debug.h"
#include "symtab.h"

struct token
{
    enum token_type tokenType; //high-level token type: e.g. delimiter, operator, etc
    enum data_type dataType;   //data type of variable tokens : e.g. int, char, long
    struct symtbr * symtype;   //pointer to the symbol's type in the symbol
                               //table: this is needed for tokens that don't have 
                               //a basic type (structs and such)
    struct symtbr * symentry;  //pointer to entry in the symbol table
    struct token* operands;
    struct token* link;
    union
    {
        char tokenstring[MAX_TOKEN_STRING_LENGTH]; //contains the identifier name or string constant value
        unsigned int which; //The whichval further disambiguates what subtype
                            //of token we have. E.g. what type of OPERATOR_TOKEN we are looking at
        uint64_t integer_number;    //contains the value of an integer constant
        double floating_number; //contains the value of a floating point constant
    } tokenvalue;

};

#define stringval  tokenvalue.tokenstring
#define whichval tokenvalue.which
#define intval tokenvalue.integer_number
#define realval tokenvalue.floating_number

//Token allocation function; either we succeed or we die (b/c we are out of
//memory)
TOKEN make_token(void)
{
    TOKEN t = calloc(1, sizeof(struct token));
    if(t == NULL)
    {
        fprintf(stderr, "Token allocation failed. are we out of memory?\n");
        exit(-1);
    }
    return t;
}

//sets the high-level type of the token; the types correspond to broad
//categories like delimiters, keywords, number constants, and strings
void set_token_type(TOKEN t, TokenType tType)
{
    t->tokenType = tType;
}

//returns the high-level type of the token; e.g. operator, delimiter,
//string constant, etc
TokenType get_token_type(TOKEN t)
{
    return t->tokenType;
}

void set_token_data_type(TOKEN t, DataType dType)
{
    t->dataType = dType;
}

DataType get_token_data_type(TOKEN t)
{
    return t->dataType;
}

void set_token_string_value(TOKEN t, char* string)
{
    int i = 0;

    for(i = 0; (i < MAX_TOKEN_STRING_LENGTH) && (string[i] != 0); i++)
    {
        t->stringval[i] = string[i];
    }
    t->stringval[i] = 0; //terminate the string FIXME: there may be an off-by-one buffer overflow here...
}

char* get_token_string_value(TOKEN t)
{
    char* s;
    if(t->tokenType == KEYWORD_TOKEN)
    {
        //FIXME: this is a hack to make looking up basic types in the symbol table work
        switch(t->whichval)
        {
            case CHAR:
                s = "char";
                break;
            case SHORT:
                s = "short";
                break;
            case INT:
                s = "int";
                break;
            case LONG:
                s = "long";
                break;
            default:
                s = "unknown";
                break;
        }
    }
    else
    {
        s = t->stringval;
    }
    return s;
}

//sets the subtype of the token: e.g. an OPERATOR_TOKEN might have a subtype of ADDITION
void set_token_subtype(TOKEN t, int which)
{
    t->whichval = which;
}

//disambiguates the token type by returning the subtype of the token: e.g. an
//OPERATOR_TOKEN might have a subtype of ADDITION
unsigned int get_token_subtype(TOKEN t)
{
    return t->whichval;
}

//sets the value of a token representing an integer constant
void set_token_integer_value(TOKEN t, uint64_t intvalue)
{
    t->intval = intvalue;
}

//returns the value of a token representing an integer constant
uint64_t get_token_integer_value(TOKEN t)
{
    return t->intval;
}

//gets the next token in a linked-list of tokens
TOKEN get_token_link(TOKEN tok)
{
    return tok->link;
}

//links two tokens together to form a list (or as part of a tree structure)
void set_token_link(TOKEN tok, TOKEN linked_tok)
{
    tok->link = linked_tok;
}

//gets the associated operands from an OPERATOR_TOKEN; together, they form a
//tree that forms an expression. The trees can be connected together to form
//arbitrarily complex expressions.
TOKEN get_token_operands(TOKEN tok)
{
    return tok->operands;
}

//attaches an operand token (an identifier or a constant) to the operator to 
//form an expression tree
void set_token_operands(TOKEN tok, TOKEN operand)
{
    tok->operands = operand;
}

//returns the type of the symbol for the given token. This returns type information
//for datatypes that are not built in (such as user defined structs)
SYMBOL get_token_symbol_type(TOKEN tok)
{
    SYMBOL result = NULL;
    if(tok != NULL)
    {
        result = tok->symtype;
    }
    return result;
}

//sets the type of the token (most likely some sort of user defined datatype.
//i.e. not an char, short, int, etc) to the type represented by the given
//SYMBOL object
void set_token_symbol_type(TOKEN tok, SYMBOL sym_t)
{
    tok->symtype = sym_t;
}

//gets the entry in the symbol table corresponding to the given token
//this helps us get additional semantic information about the token
SYMBOL get_token_symbol_table_entry(TOKEN tok)
{
    return tok->symentry;
}

//sets a link between a token in our parse tree and the symbol table, 
//effectively annotating the tree node with semantic information
void set_token_symbol_table_entry(TOKEN tok, SYMBOL sym_entry)
{
    tok->symentry = sym_entry;
}

double get_token_floating_point_value(TOKEN tok)
{
    return tok->realval;
}

void set_token_floating_point_value(TOKEN tok, double value)
{
    tok->realval = value;
}

//gets the storage class from a token containing a storage class reserved word
//(e.g. extern, static)
StorageClass get_token_storage_class(TOKEN tok)
{
    if( false == token_matches_keyword(tok, EXTERN) && false == token_matches_keyword(tok, TYPEDEF) &&
            false == token_matches_keyword(tok, STATIC))
    {
        printf("You tried to get a storage class from a non-storage-class token\n");
        exit(EXIT_FAILURE); //FIXME: we don't want to just barf in the future, but for now its ok
    }

    int whichStorage = get_token_subtype(tok);
    StorageClass s;

    switch(whichStorage)
    {
        case EXTERN:
            s = EXTERNAL_STORAGE_CLASS;
            break;
        case TYPEDEF:
            s = TYPEDEF_STORAGE_CLASS;
            break;
        case STATIC:
            s = STATIC_STORAGE_CLASS;
            break;
        default: //automatic variables and everything else
            s = UNKNOWN_STORAGE_CLASS;
            break;
    }
    return s;
}


//RANDOM HELPER FUNCTIONS: FIXME figure out where these functions ought to go instead of just jamming them all down here
//perhaps consider putting them with the other token operations?

bool is_keyword_token(TOKEN tok)
{
    bool result = false;
    if(get_token_type(tok) == KEYWORD_TOKEN)
    {
        result = true;
    }
    return result;
}

bool token_matches_keyword(TOKEN tok, KeywordType desired_keyword)
{
    bool result = false;
    if(is_keyword_token(tok) && (get_token_subtype(tok) == desired_keyword))
    {
        result = true;
    }
    return result;
}

bool is_delimiter_token(TOKEN tok)
{
    bool result = false;
    if(get_token_type(tok) == DELIMITER_TOKEN)
    {
        result = true;
    }
    return result;
}

bool token_matches_delimiter(TOKEN tok, DelimiterType desired_delimiter)
{
    bool result = false;
    if(is_delimiter_token(tok) && (get_token_subtype(tok) == desired_delimiter))
    {
        result = true;
    }
    return result;
}

bool is_operator_token(TOKEN tok)
{
    bool result = false;
    if(get_token_type(tok) == OPERATOR_TOKEN)
    {
        result = true;
    }
    return result;
}

bool token_matches_operator(TOKEN tok, OperatorType desired_operator)
{
    bool result = false;
    if(is_operator_token(tok) && (get_token_subtype(tok) == desired_operator))
    {
        result = true;
    }
    return result;
}


//checks to see if the token is an assignment type operator 
//like "=" or "+=" or any of the other variants
bool is_assignment_operator_token(TOKEN tok)
{
    bool result = false;
    if((true == token_matches_operator(tok, ASSIGNMENT)) ||
       (true == token_matches_operator(tok, PLUS_EQUAL)) ||
       (true == token_matches_operator(tok, MINUS_EQUAL)) ||
       (true == token_matches_operator(tok, MULTIPLY_EQUAL)) ||
       (true == token_matches_operator(tok, DIVIDE_EQUAL)) ||
       (true == token_matches_operator(tok, MOD_EQUAL)) ||
       (true == token_matches_operator(tok, BITWISE_AND_EQUAL)) ||
       (true == token_matches_operator(tok, BITWISE_OR_EQUAL)))
    {
        result = true;
    }
    return result;
}

//checks to see if the operator is one of the possible unary operators
bool is_unary_operator_token(TOKEN tok)
{
    bool result = false;
    if((true == token_matches_operator(tok, AMPERSAND)) || 
       (true == token_matches_operator(tok, STAR)) ||
       (true == token_matches_operator(tok, ADDITION)) ||
       (true == token_matches_operator(tok, SUBTRACTION)) ||
       (true == token_matches_operator(tok, BITWISE_NOT)) ||
       (true == token_matches_operator(tok, BOOLEAN_NOT)))
    {
        result = true;
    }
    return result;
}

//checks to see if we have a *, a / or a %
bool is_multiplicative_operator_token(TOKEN tok)
{
    bool result = false;
    if(true == token_matches_operator(tok, STAR) ||  
            true == token_matches_operator(tok, MULTIPLICATION) ||
            true == token_matches_operator(tok, DIVISION) ||
            true == token_matches_operator(tok, MODULAR_DIVISION) )
    {
        result = true;
    }
    return result;
}

bool is_additive_operator_token(TOKEN tok)
{
    bool result = false;
    if((true == token_matches_operator(tok, ADDITION)) ||
            (true == token_matches_operator(tok, SUBTRACTION)))
    {
        result = true;
    }
    return result;
}

bool is_iteration_keyword_token(TOKEN tok)
{
    bool result = false;
    if( true == token_matches_keyword(tok, WHILE) ||
            true == token_matches_keyword(tok, FOR) ||
            true == token_matches_keyword(tok, DO) )
    {
        result = true;
    }
    return result;
}

bool is_jump_keyword_token(TOKEN tok)
{
    bool result = false;
    if( (true == token_matches_keyword(tok, GOTO)) || 
            (true == token_matches_keyword(tok, CONTINUE)) || 
            (true == token_matches_keyword(tok, BREAK)) ||
            (true == token_matches_keyword(tok, RETURN)) )
    {
        result = true;
    }
    return result;
}

bool is_selection_keyword_token(TOKEN tok)
{
    bool result = false;
    if( (true == token_matches_keyword(tok, IF)) || 
            (true == token_matches_keyword(tok, SWITCH)))
    {
        result = true;
    }
    return result;
}


/************************** TOKEN PRINTING FUNCTIONS *************************************************/

void print_token(TOKEN t)
{
    if( NULL != t )
    {
        printf("\n");
        print_token_type(t);
        switch(get_token_type(t))
        {
            case KEYWORD_TOKEN:
                print_token_keyword_type(t);
                break;
            case IDENTIFIER_TOKEN:
                print_identifier_token(t);
                break;
            case STRING_LITERAL:
                printf("The string is \"%s\"\n", get_token_string_value(t));
                break;
            case DELIMITER_TOKEN:
                print_token_delimiter_type(t);
                break;
            case NUMBER_TOKEN:
                print_token_numeric_value(t);
                break;
            case OPERATOR_TOKEN:
                print_operator_token(t);
                break;
            default:
                printf("I don't know what kind of token this is...\n");
                break;
        }
    }
    else
    {
        printf("The TOKEN was empty!\n");
    }
}

void print_token_type(TOKEN t)
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

void print_token_keyword_type(TOKEN t)
{
    printf("This is a \"");
    switch(get_token_subtype(t))
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
        case DEFAULT:
            printf("default");
            break;
        case RETURN:
            printf("return");
            break;
        case GOTO:
            printf("goto");
            break;
        case EXTERN:
            printf("extern");
            break;

        default:
            printf("UNKNOWN");
            break;
    }
    printf("\" keyword token\n");
}

void print_identifier_token(TOKEN t)
{
    printf("The identifier is \"%s\"\n", get_token_string_value(t));
}

void print_token_delimiter_type(TOKEN t)
{
    printf("The DELIMITER we found is a ");
    switch(get_token_subtype(t))
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

void print_token_numeric_value(TOKEN t)
{
    printf("The value of this NUMBER_TOKEN is %lu\n", get_token_integer_value(t));
}

void print_operator_token(TOKEN t)
{
    printf("The OPERATOR_TOKEN is a ");
    switch(get_token_subtype(t))
    {
    //single character operators
        case ADDITION:
            printf("ADDITION ");
            break;
        case SUBTRACTION:
            printf("SUBTRACTION ");
            break;
        case MULTIPLICATION:
            printf("MULTIPLICATION ");
            break;
        case DIVISION:
            printf("DIVISION ");
            break;
        case MODULAR_DIVISION:
            printf("MODULAR_DIVISION ");
            break;
        case BOOLEAN_NOT:
            printf("BOOLEAN_NOT ");
            break;
        case BITWISE_NOT:
            printf("BITWISE_NOT ");
            break;
        case BITWISE_AND:
            printf("BITWISE_AND ");
            break;
        case BITWISE_OR:
            printf("BITWISE_OR ");
            break;
        case BITWISE_XOR:
            printf("BITWISE_XOR ");
            break;
        case DOT:
            printf("DOT ");
            break;
        case ASSIGNMENT:
            printf("ASSIGNMENT ");
            break;
        case GREATER_THAN:
            printf("GREATER_THAN ");
            break;
        case LESS_THAN:
            printf("LESS_THAN ");
            break;

    //two-character operators
        case EQUALS:
            printf("EQUALS ");
            break;
        case NOT_EQUALS:
            printf("NOT_EQUALS ");
            break;
        case GREATER_THAN_OR_EQUAL:
            printf("GREATER_THAN_OR_EQUAL ");
            break;
        case LESS_THAN_OR_EQUAL:
            printf("LESS_THAN_OR_EQUAL ");
            break;
        case BOOLEAN_OR:
            printf("BOOLEAN_OR ");
            break;
        case BOOLEAN_AND:
            printf("BOOLEAN_AND ");
            break;
        case SHIFT_LEFT:
            printf("SHIFT_LEFT ");
            break;
        case SHIFT_RIGHT:
            printf("SHIFT_RIGHT ");
            break;
        case INCREMENT:
            printf("INCREMENT ");
            break;
        case DECREMENT:
            printf("DECREMENT ");
            break;
        case PLUS_EQUAL:
            printf("PLUS_EQUAL ");
            break;
        case MINUS_EQUAL:
            printf("MINUS_EQUAL ");
            break;
        case MULTIPLY_EQUAL:
            printf("MULTIPLY_EQUAL ");
            break;
        case DIVIDE_EQUAL:
            printf("DIVIDE_EQUAL ");
            break;
        case MOD_EQUAL:
            printf("MOD_EQUAL ");
            break;
        case BITWISE_AND_EQUAL:
            printf("BITWISE_AND_EQUAL ");
            break;
        case BITWISE_OR_EQUAL:
            printf("BITWISE_OR_EQUAL ");
            break;
        case ARROW:
            printf("ARROW ");
            break;
        case STAR:
            printf("STAR ");
            break;
        case AMPERSAND:
            printf("AMPERSAND ");
            break;
        case PARSE_TREE_IF:                      //for internal use in the parse tree
            printf("PARSE_TREE_IF ");
            break;
        case PARSE_TREE_STATEMENT_LIST:          //for internal use in the parse tree
            printf("PARSE_TREE_STATEMENT_LIST ");
            break;
        case PARSE_TREE_LABEL:                   //for internal use in the parse tree
            printf("PARSE_TREE_LABEL ");
            break;
        case PARSE_TREE_FUNCALL:                 //for internal use in the parse tree
            printf("PARSE_TREE_FUNCALL ");
            break;
        case PARSE_TREE_ARRAY_REFERENCE:         //for internal use in the parse tree
            printf("PARSE_TREE_ARRAY_REFERENCE ");
            break;
        case PARSE_TREE_FUNCTION_DEFINITION:     //for internal use in the parse tree
            printf("PARSE_TREE_FUNCTION_DEFINITION ");
            break;
        case PARSE_TREE_TRANSLATION_UNIT:        //for internal use in the parse tree
            printf("PARSE_TREE_TRANSLATION_UNIT ");
            break;
        case PARSE_TREE_MAKE_FLOAT:              //for internal use in the parse tree
            printf("PARSE_TREE_MAKE_FLOAT ");
            break;
        case PARSE_TREE_RETURN:                  //for internal use in the parse tree
            printf("PARSE_TREE_RETURN ");
            break;
        case PARSE_TREE_GOTO:                    //for internal use in the parse tree
            printf("PARSE_TREE_GOTO ");
            break;
        default:
            printf("UNKNOWN ");
            break;
    }
    printf("operator\n");
}

/************************** END OF TOKEN PRINTING FUNCTIONS ******************************************/
