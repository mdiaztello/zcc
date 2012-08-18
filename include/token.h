#ifndef INCLUDE_GUARD__TOKEN_H_
#define INCLUDE_GUARD__TOKEN_H_

#define MAX_TOKEN_STRING_LENGTH 256 //this is somewhat arbitrary, but it seems long enough to deal with
                            //really long function and variable names


typedef struct token* TOKEN;  //opaque pointer declaration; implementation is hidden in token.c

enum token_type
{
    KEYWORD_TOKEN = 0,
    DELIMITER_TOKEN,
    STRING_LITERAL,
    CHARACTER_LITERAL,
    IDENTIFIER_TOKEN,
    OPERATOR_TOKEN,
    NUMBER_TOKEN,

    // this last enum is here just to be able to iterate across
    // all of the token types
    NUM_TOKEN_TYPES
};
typedef enum token_type TokenType;

enum operator_type
{
    //single character operators
    ADDITION = 0,           // "+"
    SUBTRACTION,            // "-"
    MULTIPLICATION,         // "*"
    DIVISION,               // "/"
    MODULAR_DIVISION,       // "%"
    BOOLEAN_NOT,            // "!"
    BITWISE_NOT,            // "~"
    BITWISE_AND,            // "&"
    BITWISE_OR,             // "|"
    BITWISE_XOR,            // "^"
    REFERENCE,              // "&"
    DEREFERENCE,            // "*"
    DOT,                    // "."
    ASSIGNMENT,             // "="
    GREATER_THAN,           // ">"
    LESS_THAN,              // "<"

    //two-character operators
    EQUALS,                 // "=="
    NOT_EQUALS,             // "!="
    GREATER_THAN_OR_EQUAL,  // ">=" 
    LESS_THAN_OR_EQUAL,     // "<="
    BOOLEAN_OR,             // "||"
    BOOLEAN_AND,            // "&&"
    SHIFT_LEFT,             //"<<",     //NOTE: I AM EXPLICITLY DISSALLOWING "<<="  
    SHIFT_RIGHT,            //">>",     //NOTE: I AM EXPLICITLY DISSALLOWING ">>="  
    INCREMENT,              //"++",   (post/pre)
    DECREMENT,              //"--",   (post/pre)
    PLUS_EQUAL,             //"+=",   
    MINUS_EQUAL,            //"-=",   
    MULTIPLY_EQUAL,         //"*=",   
    DIVIDE_EQUAL,           //"/=",   
    MOD_EQUAL,              //"%=",   
    BITWISE_AND_EQUAL,      //"&=",   
    BITWISE_OR_EQUAL,       //"|=",   
    ARROW,                  //"->"   (same as a dereference followed by a dot operator)
    //the "STAR" operator stands for both multiplication and dereferencing/pointers
    //because both operations share the same symbol, so disambiguation can't be done in the lexer
    //rather, it must be done while the statements are being parsed
    STAR,
    AMPERSAND,
    PARSE_TREE_IF,                      //for internal use in the parse tree
    PARSE_TREE_STATEMENT_LIST,          //for internal use in the parse tree
    PARSE_TREE_LABEL,                   //for internal use in the parse tree
    PARSE_TREE_FUNCALL,                 //for internal use in the parse tree
    PARSE_TREE_ARRAY_REFERENCE,         //for internal use in the parse tree
    PARSE_TREE_FUNCTION_DEFINITION,     //for internal use in the parse tree
    PARSE_TREE_TRANSLATION_UNIT,        //for internal use in the parse tree
    PARSE_TREE_MAKE_FLOAT,              //for internal use in the parse tree
    PARSE_TREE_RETURN,                  //for internal use in the parse tree
    PARSE_TREE_GOTO,                    //for internal use in the parse tree
    

    // this last enum is here just to be able to iterate across
    // all of the operator types
    NUM_OPERATOR_TYPES
};
typedef enum operator_type OperatorType;

enum delimiter_type
{
    COMMA = 0,      // ","
    SEMICOLON,      // ";"
    COLON,          // ":"
    OPEN_PAREN,     // "("
    CLOSE_PAREN,    // ")"
    OPEN_BRACKET,   // "["
    CLOSE_BRACKET,  // "]"
    OPEN_BRACE,     // "{"
    CLOSE_BRACE,    // "}"

    // this last enum is here just to be able to iterate across
    // all of the delimiter types
    NUM_DELIMITER_TYPES
};
typedef enum delimiter_type DelimiterType;

enum data_type 
{
    INTEGER = 0,
    //UNSIGNED_INT8,
    //UNSIGNED_INT16,
    //UNSIGNED_INT32,
    //UNSIGNED_INT64,
    //SIGNED_INT8,
    //SIGNED_INT16,
    //SIGNED_INT32,
    //SIGNED_INT64,
    FLOATING_POINT,
    //FLOAT32,
    //FLOAT64,
    STRING_TYPE,
    BOOLEAN_TYPE,

    // this last enum is here just to be able to iterate across
    // all of the number types
    NUM_DATA_TYPES
};
typedef enum data_type DataType;

#define MAX_KEYWORD_LENGTH 9 //"continue" and "unsigned" are the longest keywords
                                //with lengths of 8 characters (+ 1 for the null terminal)
enum keyword_types
{
    STATIC = 0,
    CONST,
    UNSIGNED, 
    SIGNED, 
    CHAR, 
    SHORT, 
    INT, 
    LONG,
    FLOAT,
    DOUBLE,
    STRUCT,
    UNION,
    ENUM,
    VOID,
    TYPEDEF,
    IF,
    ELSE,
    DO,
    WHILE,
    FOR,
    CONTINUE,
    SWITCH,
    CASE,
    BREAK,
    DEFAULT,
    RETURN,
    GOTO,
    EXTERN,

    // this last enum is here just to be able to iterate across
    // all of the keyword types
    NUM_KEYWORD_TYPES
};
typedef enum keyword_types KeywordType;

#endif // INCLUDE_GUARD__TOKEN_H_
