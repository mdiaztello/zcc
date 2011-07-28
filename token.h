#ifndef __TOKEN_H_
#define __TOKEN_H_

#define MAX_TOKEN_LENGTH 256 //this is somewhat arbitrary, but it seems long enough to deal with
                            //really long function and variable names

typedef struct token* TOKEN;

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
    ADDITION = 0,           // "+"
    SUBTRACTION,            // "-"
    MULTIPLICATION,         // "*"
    DIVISION,               // "/"
    MODULAR_DIVISION,       // "%"
    BOOLEAN_NOT,            // "!"
    BOOLEAN_OR,             // "||"
    BOOLEAN_AND,            // "&&"
    BITWISE_NOT,            // "~"
    BITWISE_AND,            // " & "
    BITWISE_OR,             // " | "
    BITWISE_XOR,            // "^"
    REFERENCE,              // "&"
    DEREFERENCE,            // "*"
    DOT,                    // "."
    ASSIGNMENT,             // "="
    EQUALS,                 // "=="
    NOT_EQUALS,             // "!="
    GREATER_THAN,           // ">"
    GREATER_THAN_OR_EQUAL,  // ">=" 
    LESS_THAN,              // "<"
    LESS_THAN_OR_EQUAL,     // "<"

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
typedef enum delimeter_type DelimiterType;

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
    RETURN,
    GOTO,

    // this last enum is here just to be able to iterate across
    // all of the keyword types
    NUM_KEYWORD_TYPES
};
typedef enum keyword_types KeywordType;

TOKEN makeToken(void);
void setTokenType(TOKEN t, enum token_type tType);
TokenType getTokenType(TOKEN t);
void setDataType(TOKEN t, DataType dType);
DataType getDataType(TOKEN t);

void setStringVal(TOKEN t, char* string);
char* getStringVal(TOKEN t);

int getWhichVal(TOKEN t);
void setWhichVal(TOKEN t, int which);

void printToken(TOKEN t);
void printTokenType(TOKEN t);
void printKeywordType(TOKEN t);
void printIdentifier(TOKEN t);


#endif // __TOKEN_H_
