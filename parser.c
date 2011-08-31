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
#include "token_API.h"
#include "lexer.h"
#include <stdio.h> //for debugging purposes
#include <stdlib.h> //for exit()
#include "global_defs.h"
#include "symtab.h"
BOOLEAN isKeyword(TOKEN tok);
TOKEN translation_unit(void);
BOOLEAN reserved(TOKEN tok, KeywordType keyword);
TOKEN external_declaration(void);
TOKEN function_definition(void);
TOKEN declaration(SYMBOL s);
TOKEN declaration_specifiers(SYMBOL s);
TOKEN init_declarator_list(SYMBOL s);
TOKEN storage_class_specifier(void);
TOKEN type_specifier(void);
TOKEN init_declarator(SYMBOL s);
BOOLEAN operator(TOKEN tok, OperatorType operator);
BOOLEAN isOperator(TOKEN tok);
BOOLEAN isDelimiter(TOKEN tok);
BOOLEAN delimiter(TOKEN tok, DelimiterType delim);
TOKEN pointer(void);
TOKEN direct_declarator(SYMBOL s);
TOKEN identifier(void);
TOKEN declarator(SYMBOL s);

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
    TOKEN tok = peektok();
    beacon();printToken(tok);
    if(tok != NULL)
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
    SYMBOL sym = symalloc(); //symbol for the declaration we are dealing with
    //used as a temporary place to aggregate data about the declaration until
    //we can install the symbol
    TOKEN ext_declaration = function_definition();
    if( NULL == ext_declaration )
    {
        ext_declaration = declaration(sym);
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

TOKEN declaration_list(SYMBOL s)
{
    TOKEN dec_list = declaration(s);
    if( dec_list != NULL)
    {
        //FIXME this might be a bug, but I'm not sure yet
        setLink(dec_list, declaration_list(s));
    }
    return dec_list;
}

// <declaration> ::= <declaration-specifiers> <init-declarator-list>? ;

TOKEN declaration(SYMBOL s)
{
    beacon();
    //TOKEN dec =
    s->kind = VARSYM; //if we have made it this far, i think it's safe to assume we are declaring a variable      
    declaration_specifiers(s);

    init_declarator_list(s); //optional
    beacon();
    TOKEN t = gettok(); //consume the SEMICOLON
    printToken(t);

    return t;
}

// <declaration-specifiers> ::= <storage-class-specifier> <declaration-specifiers>? |
//                              <type-specifier> <declaration-specifiers>? |
//                              <type-qualifier> <declaration-specifiers>? |
//                              <function-specifier> <declaration-specifiers>?

TOKEN declaration_specifiers(SYMBOL s)
{
    beacon();
    TOKEN t = NULL;
    TOKEN storage_class = storage_class_specifier();
    setStorageClass(s, storage_class);
    TOKEN type_spec = type_specifier();
    printToken(type_spec);
    
    SYMBOL type = searchst(getStringVal(type_spec));
    s->basicdt = type->basicdt;
    s->datatype = type;
    s->size = type->size;
    //TOKEN type_qual = type_qualifier();
    //TOKEN function_spec = function_specifier();
    beacon();
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
        fprintf(stderr, "no type specifier found...\n");
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

TOKEN init_declarator_list(SYMBOL s)
{
    TOKEN init_dec_list = init_declarator(s);
    beacon();
    TOKEN tok = peektok();
    printToken(tok);
    if( TRUE == delimiter(tok, COMMA) )
    {
        //FIXME: this might be a bug but i'm not sure yet
        tok = gettok(); //consume the COMMA
        setLink(init_dec_list, init_declarator(s));
    }
    else
    {
        //FIXME: this might be a bug but i'm not sure yet
        setLink(init_dec_list, NULL);
    }
    beacon();
    return init_dec_list;
}

// <init-declarator> ::= <declarator> |
//                       <declarator> = <initializer>

TOKEN init_declarator(SYMBOL s)
{
    TOKEN decl = declarator(s);
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

TOKEN declarator(SYMBOL s)
{
    TOKEN ptr = NULL;
    TOKEN direct_dec = NULL;
    ptr = pointer();
    direct_dec = direct_declarator(s);

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

TOKEN direct_declarator(SYMBOL s)
{
    beacon();
    TOKEN direct_decl = identifier();
    
    SYMBOL entry = searchins(getStringVal(direct_decl));
    setSymbolNameString(s, entry->namestring);
    copy_symbol(s, entry);
    
    //can we free SYMBOL s here? or will it cause problems?

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

BOOLEAN isDelimiter(TOKEN tok)
{
    BOOLEAN result = FALSE;
    if(getTokenType(tok) == DELIMITER_TOKEN)
    {
        result = TRUE;
    }
    return result;
}

BOOLEAN delimiter(TOKEN tok, DelimiterType delim)
{
    BOOLEAN result = FALSE;
    if(isDelimiter(tok) && (getWhichVal(tok) == delim))
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
