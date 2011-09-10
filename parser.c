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
#include "error_handlers.h"
#include "statements.h"
#include "parser.h"
#include "pprint.h"



//parses the program
TOKEN parse(void)
{
    TOKEN parse_tree = translation_unit();
    parse_tree = make_translation_unit(parse_tree);
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
    TOKEN ext_declaration = NULL;
    SYMBOL sym = symalloc(); //symbol for the declaration we are dealing with
    //used as a temporary place to aggregate data about the declaration until
    //we can install the symbol

    //NOTE: we check for a global declaration first because we can easily use
    //the information we aggregate in the SYMBOL sym and pass it on rather than
    //having to backtrack like we would if we looked for a function definition first
    setStorageClass(sym, STATIC_STORAGE_CLASS);
    ext_declaration = declaration(sym);
    if( NULL == ext_declaration )
    {
        //we have a global variable declaration instead of a function definition
        //and global variables default to external storage
        setStorageClass(sym, EXTERNAL_STORAGE_CLASS); 
        ext_declaration = function_definition(sym);
    }

    return ext_declaration;
}

//parses function-definitions
// <function-definition> ::= <declaration-specifiers> <declarator> <declaration-list>? <compound-statement>
// a compound-statement is a block

TOKEN function_definition(SYMBOL s)
{
    TOKEN func_definition = NULL;
    //we didn't find a global var so assume we found a function
    //we already parsed the declaration specifiers and the declarator in the earlier parsing attempt
    //so just repurpose the information we collected instead of backtracking and re-parsing

    expect(DELIMITER_TOKEN, OPEN_PAREN, NO_ERROR_HANDLER);
    TOKEN params = parameter_type_list(); //we already found the return type and function name earlier, so we just pick up from there
    expect(DELIMITER_TOKEN, CLOSE_PAREN, NO_ERROR_HANDLER);
    SYMBOL return_type = s;
    insertfn(s->namestring, return_type, getSymbolType(params)); //FIXME: extend this to more than just one parameter
    TOKEN func_body = compound_statement();

    //FIXME: this is a hack to retrieve the function name in TOKEN form
    //figure out a better way to deal with this
    TOKEN func_name = makeToken();
    setTokenType(func_name, IDENTIFIER_TOKEN);
    setStringVal(func_name, s->namestring);
    setSymbolTableEntry(func_name, s);
    params = make_statement_list(params);
    func_definition = make_function_definition(func_name, params, func_body);
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
    TOKEN t = NULL;
    TOKEN dec = NULL;
    s->kind = VARSYM; //if we have made it this far, i think it's safe to assume we are declaring a variable      
    declaration_specifiers(s);
    dec = init_declarator_list(s);
    t = peektok();

    if(FALSE == delimiter(t, SEMICOLON))
    {
        //it turns out that we haven't found a variable declaration, so exit
        //and allow the function definition recognizer take a stab at parsing
        dec = NULL;
    }
    else
    {
        t = gettok(); //consume the SEMICOLON
        SYMBOL entry = searchins(s->namestring);
        copy_symbol(s, entry);
    }

    return dec;
}

// <declaration-specifiers> ::= <storage-class-specifier> <declaration-specifiers>? |
//                              <type-specifier> <declaration-specifiers>? |
//                              <type-qualifier> <declaration-specifiers>? |
//                              <function-specifier> <declaration-specifiers>?

TOKEN declaration_specifiers(SYMBOL s)
{
    TOKEN t = NULL;
    //check the storage class of the thing being declared
    //if the storage class is specified, else use the default
    //that is already inside the symbol
    TOKEN storage_class = storage_class_specifier();
    if( NULL != storage_class )
    {
        setStorageClass(s, getTokenStorageClass(storage_class));
    }
    TOKEN type_spec = type_specifier();
    
    SYMBOL type = searchst(getStringVal(type_spec));
    s->basicdt = type->basicdt;
    s->datatype = type;
    s->size = type->size;
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
    TOKEN tok = peektok();
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
    TOKEN direct_decl = identifier();
    setSymbolNameString(s, getStringVal(direct_decl));
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


// <parameter-type-list> ::= <parameter-list> |
//                           <parameter-list> , ... //FIXME: NO SUPPORT FOR VARIADIC FUNCTIONS FOR THE MOMENT

TOKEN parameter_type_list(void)
{
    TOKEN param_list = parameter_list();
    return param_list;
}

// <parameter-list> ::= <parameter-declaration> <parameter-list>*
//

TOKEN parameter_list(void)
{
    TOKEN param_list = parameter_declaration();
    TOKEN tok = peektok();
    if( TRUE == delimiter(tok, COMMA) )
    {
        //FIXME: this might be a bug but i'm not sure yet
        tok = gettok(); //consume the COMMA
        setLink(param_list, parameter_list());
    }
    else
    {
        //FIXME: this might be a bug but i'm not sure yet
        setLink(param_list, NULL);
    }
    return param_list;
}

// <parameter-declaration> ::= <declaration-specifiers> <declarator> |
//                             <declaration-specifiers> <abstract-declarator>

TOKEN parameter_declaration(void)
{
    SYMBOL s = symalloc();
    TOKEN dec = NULL;
    declaration_specifiers(s);
    dec = declarator(s);
    setSymbolType(dec,s);

    return dec;
}


void expect(TokenType tType, unsigned int whichToken, void (*errorAction)(void))
{
    TOKEN peek = peektok();
    if((tType != getTokenType(peek)) || (whichToken != getWhichVal(peek)))
    {
        if(NULL == errorAction)
        {
            printf("ERROR: an unexpected token was encountered on line %5lu of the source file\n", getSourceLine());
        }
        else
        {
            errorAction();
        }
        //if(recovery options are set)
        //{
        // do recovery stuff
        // }
        // else
        // barf
        exit(EXIT_FAILURE);
    }
    gettok();
}
