/*
 * =====================================================================================
 *
 *       Filename:  parser.c
 *
 *    Description:  This is the recursive descent parser for the compiler
 *
 *        Version:  1.0
 *        Created:  7/21/2011 10:24:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michael Diaz-Tello (MDT), mdiaz.tello@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */


#include "debug.h"
#include "token.h"
#include "lexer.h"
#include <stdio.h> //for debugging purposes
#include <stdlib.h> //for exit()
#include "global_defs.h"
BOOLEAN isKeyword(TOKEN tok);
TOKEN translation_unit(void);
BOOLEAN reserved(TOKEN tok, KeywordType keyword);
TOKEN external_declaration(void);
TOKEN function_definition(void);
TOKEN declaration(void);
TOKEN declaration_specifiers(void);
TOKEN init_declarator_list(void);
TOKEN storage_class_specifier(void);
TOKEN type_specifier(void);
TOKEN init_declarator(void);
BOOLEAN operator(TOKEN tok, OperatorType operator);
BOOLEAN isOperator(TOKEN tok);
TOKEN direct_declarator(void);
TOKEN pointer(void);
TOKEN direct_declarator(void);
TOKEN identifier(void);
TOKEN declarator(void);

//parses the program
TOKEN parse(void)
{
    TOKEN parse_tree = translation_unit();
    return parse_tree;
}

//parses a "translation unit" which is a .c or .h file that
//has already been pre-processed
// <translation-unit> ::= <external-declaration> <translation-unit>*
// i.e. a translation unit is a list of external declarations

TOKEN translation_unit(void)
{
    TOKEN trans_unit = NULL;
    trans_unit = external_declaration(); 
    if(trans_unit != NULL)
    {
        setLink(trans_unit, translation_unit());
    }
    return trans_unit;
}

//parses external-declarations
// <external-declaration> ::= <function-definition> | 
//                           <declaration>
//i.e. an external-declaration is either a function definition
// OR it is a declaration

TOKEN external_declaration(void)
{
    beacon();
    TOKEN ext_declaration = function_definition();
    if( NULL == ext_declaration )
    {
        ext_declaration = declaration();
    }

    return ext_declaration;
}

//parses function-definitions
// <function-definition> ::= <declaration-specifiers> <declarator> <declaration-list>? <compound-statement>
// a compound-statement is a block

TOKEN function_definition(void)
{
    TOKEN func_definition = NULL;
    //declaration_specifiers();
    //declarator();
    //declaration_list(); //FIXME: This allows K&R style function declarations. I may want to disallow this.
    //if(declaration_list == NULL) //blah
    //block();
    beacon();
    return func_definition;
}


// <declaration-list> ::= <declaration> <declaration-list>*

TOKEN declaration_list(void)
{
    TOKEN dec_list = declaration();
    if( dec_list != NULL)
    {
        setLink(dec_list, declaration_list());
    }
    return dec_list;
}

// <declaration> ::= <declaration-specifiers> <init-declarator-list>? ;

TOKEN declaration(void)
{
    beacon();
    //TOKEN dec =
    declaration_specifiers();

    init_declarator_list(); //optional
    TOKEN t = gettok(); //consume the SEMICOLON
    printToken(t);

    return t;
}

// <declaration-specifiers> ::= <storage-class-specifier> <declaration-specifiers>? |
//                              <type-specifier> <declaration-specifiers>? |
//                              <type-qualifier> <declaration-specifiers>? |
//                              <function-specifier> <declaration-specifiers>?

TOKEN declaration_specifiers(void)
{
    beacon();
    TOKEN t = NULL;
    TOKEN storage_class = storage_class_specifier();
    printToken(storage_class);
    TOKEN type_spec = type_specifier();
    printToken(type_spec);
    //TOKEN type_qual = type_qualifier();
    //TOKEN function_spec = function_specifier();

    return t;
}

// <storage-class-specifier> ::= TYPEDEF |
//                               EXTERN  |
//                               STATIC  |
//                               AUTO    |  NOTE: I may disallow this
//                               REGISTER   NOTE: I will explicitly disallow this
//      only one storage class specifier allowed at a time

TOKEN storage_class_specifier(void)
{
    TOKEN storage_class = NULL;
    TOKEN tok = peektok();
    if(tok == NULL)
    {
        return NULL;
    }

    if(TRUE == reserved(tok, TYPEDEF) ||
            TRUE == reserved(tok, EXTERN) ||
            TRUE == reserved(tok, STATIC))
    {
        storage_class = gettok();
    }
    else
    {
        storage_class = NULL;
    }

    return storage_class;
}


// <type-specifier> ::= VOID |
//                      CHAR |
//                      SHORT |
//                      INT |
//                      LONG |
//                      FLOAT |
//                      DOUBLE |
//                      SIGNED |
//                      UNSIGNED |
//                      _BOOL   |
//                      _COMPLEX |
//                      struct-or-union-specifier |
//                      enum-specifier |
//                      typedef-name
//

TOKEN type_specifier(void)
{
    beacon();
    TOKEN tok = peektok();
    if(tok == NULL)
    {
        return NULL;
    }

    TOKEN type_spec;

    if( TRUE == reserved(tok, VOID) ||
            TRUE == reserved(tok, CHAR) ||
            TRUE == reserved(tok, SHORT) ||
            TRUE == reserved(tok, INT) ||
            TRUE == reserved(tok, LONG) ||
            //TRUE == reserved(tok, FLOAT) ||
            //TRUE == reserved(tok, DOUBLE) ||
            TRUE == reserved(tok, SIGNED) ||
            TRUE == reserved(tok, UNSIGNED) )
    {
        type_spec = gettok();
    }
    else
    {
        type_spec = NULL;
    }

    return type_spec;
}



// <init-declarator-list> ::= <init-declarator> |
//                             <init-declarator> , <init-declarator-list>

TOKEN init_declarator_list(void)
{
    TOKEN init_dec_list = init_declarator();
    if( NULL != init_dec_list )
    {
        setLink(init_dec_list, init_declarator());
    }
    return init_dec_list;
}

// <init-declarator> ::= <declarator> |
//                       <declarator> = <initializer>

TOKEN init_declarator(void)
{
    TOKEN decl = declarator();
    TOKEN tok = peektok();
    if(tok == NULL)
    {
        return NULL;
    }
    TOKEN initial_value = NULL;
    if(TRUE == operator(tok, ASSIGNMENT))
    {
        initial_value = NULL;//initializer();
    }
    return decl;
}


// <declarator> ::= <pointer>? <direct-declarator>

TOKEN declarator(void)
{
    TOKEN ptr = NULL;
    TOKEN direct_dec = NULL;
    ptr = pointer();
    direct_dec = direct_declarator();

    return direct_dec;
}

// <pointer> ::=  * <type-qualifier-list>? <pointer>*

//stub for now
TOKEN pointer(void)
{
    return NULL;
}

// <direct-declarator> ::= <identifier> |
//                         ( <declarator> ) |
//                         <direct-declarator> [ <type-qualifier-list>? <assignment-expression>? ] |
//                         <direct-declarator> [ STATIC <type-qualifier-list>? <assignment-expression> ] |
//                         <direct-declarator> [ <type-qualifier-list> STATIC <assignment-expression> ] |
//                         <direct-declarator> [ <type-qualifier-list>? * ] |
//                         <direct-declarator> ( <parameter-type-list> )
//                         <direct-declarator> ( <identifier-list>? )

TOKEN direct_declarator(void)
{
    TOKEN direct_decl = identifier();
    printToken(direct_decl);
    return direct_decl;
}

//<identifier> ::= <identifier-nodigit> |
//                 <identifier> <identifier-non-digit> | 
//                 <identifier> <digit>
//NOTE: The buck stops here. I already wrote a lexer that can
//tokenize C identifiers, so I will simply use my gettok() routine

TOKEN identifier(void)
{
    TOKEN tok = peektok(); 
    if(tok == NULL)
    {
        return NULL;
    }
    if( getTokenType(tok) == IDENTIFIER_TOKEN )
    {
        tok = gettok();
    }
    else
    {
        tok = NULL;
    }
    return tok;
}

BOOLEAN isKeyword(TOKEN tok)
{
    BOOLEAN result = FALSE;
    if(getTokenType(tok) == KEYWORD_TOKEN)
    {
        result = TRUE;
    }
    return result;
}

BOOLEAN reserved(TOKEN tok, KeywordType keyword)
{
    BOOLEAN result = FALSE;
    if(isKeyword(tok) && (getWhichVal(tok) == keyword))
    {
        result = TRUE;
    }
    return result;
}

BOOLEAN isOperator(TOKEN tok)
{
    BOOLEAN result = FALSE;
    if(getTokenType(tok) == OPERATOR_TOKEN)
    {
        result = TRUE;
    }
    return result;
}

BOOLEAN operator(TOKEN tok, OperatorType operator)
{
    BOOLEAN result = FALSE;
    if(isOperator(tok) && (getWhichVal(tok) == operator))
    {
        result = TRUE;
    }
    return result;
}
