/*
 * =====================================================================================
 *
 *       Filename:  statements.c
 *
 *    Description:  This file contains all of the parsing routines for the various
 *    statement productions. Statement productions are basically all of the control
 *    structures of the language (loops, conditionals) but they can also be expressions.
 *    However, expression parsing will be handled elsewhere.
 *
 *        Version:  1.0
 *        Created:  2011/09/03 00:23:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michael Diaz-Tello (MDT), mdiaz.tello@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include "token.h"
#include "token_API.h"
#include "lexer.h"
#include "statements.h"
#include "expressions.h"
#include "parser.h"
#include "error_handlers.h"
#include <stdio.h>
#include "debug.h"
#include "pprint.h"

// <statement> ::= <labeled-statement> |
//                 <compound-statement> |
//                 <expression-statement> |
//                 <selection-statement> |
//                 <iteration-statement> |
//                 <jump-statement>

void statement(void)
{
    TOKEN tok = peektok();

    if((getTokenType(tok) == IDENTIFIER_TOKEN) || 
            (TRUE == reserved(tok, CASE)) || 
            (TRUE == reserved(tok, DEFAULT)) || 
            (TRUE == delimiter(tok, OPEN_PAREN)) ||
            (TRUE == delimiter(tok, SEMICOLON)))
    {
        //labeled_statement();
        expression_statement();
    }
    else if(TRUE == delimiter(tok, OPEN_BRACE))
    {
        compound_statement();
    }
    else if( (TRUE == reserved(tok, IF)) || (TRUE == reserved(tok, SWITCH)) )
    {
        //selection_statement();
    }
    else if( (TRUE == reserved(tok, WHILE)) || (TRUE == reserved(tok, DO)) || (TRUE == reserved(tok, FOR)) )
    {
        //iteration_statement();
    }
    else if( (TRUE == reserved(tok, GOTO)) || (TRUE == reserved(tok, CONTINUE)) || (TRUE == reserved(tok, BREAK)) )
    {
        //jump_statement();
    }
    else //we must have a primary expression
    {
        expression_statement();
        //primary_expression();//FIXME this may be the incorrect action
        //expect(DELIMITER_TOKEN, SEMICOLON, NULL);
    }

}


// <compound-statement> ::= { <block-item-list>* }
//

//FIXME: stub for now
void compound_statement(void)
{
    startBlock();
    expect(DELIMITER_TOKEN, OPEN_BRACE, compound_statement_error_handler);
    block_item_list();
    expect(DELIMITER_TOKEN, CLOSE_BRACE, NULL);
    endBlock();
}


// <block-item-list> ::= <block-item> <block-item-list>*
//

void block_item_list(void)
{
    block_item();
    if(FALSE == delimiter(peektok(), CLOSE_BRACE))
    {
        block_item_list();
    }
}

// <block-item> ::= <declaration> |
//                  <statement>

void block_item(void)
{
    SYMBOL sym = symalloc();
    setStorageClass(sym, AUTO_STORAGE_CLASS);
    //to see if we have a declaration, for now we will check in the symbol table to see if the token
    //we are peeking at is a basic type. If it is, we will assume we have a declaration instead of a statement
    TOKEN tok = peektok();
    SYMBOL s = searchst(getStringVal(tok));
    if (s != NULL && s->kind == BASICTYPE)
    {
        declaration(sym);
    }
    else
    {
        statement();
    }

}

// <expression-statement> ::= <expression>? ;

void expression_statement(void)
{
    TOKEN exp = expression();
    expect(DELIMITER_TOKEN, SEMICOLON, NULL);
    printf("found statement\n");
    ppexpr(exp);
}
