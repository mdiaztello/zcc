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
#include <stdlib.h>



// <statement> ::= <labeled-statement> |
//                 <compound-statement> |
//                 <expression-statement> |
//                 <selection-statement> |
//                 <iteration-statement> |
//                 <jump-statement>

TOKEN statement(void)
{
    TOKEN result = NULL;
    TOKEN tok = peektok();

    if((getTokenType(tok) == IDENTIFIER_TOKEN) || 
            (TRUE == reserved(tok, CASE)) || 
            (TRUE == reserved(tok, DEFAULT)) || 
            (TRUE == delimiter(tok, OPEN_PAREN)) ||
            (TRUE == delimiter(tok, SEMICOLON)))
    {
        //labeled_statement();
        result = expression_statement();
    }
    else if(TRUE == delimiter(tok, OPEN_BRACE))
    {
        result = compound_statement();
    }
    else if(TRUE == isSelectionKeyword(tok))
    {
        result = selection_statement();
    }
    else if(TRUE == isIterationKeyword(tok))
    {
         result = iteration_statement();
    }
    else if(TRUE == isJumpKeyword(tok))
    {
        result = jump_statement();
    }
    else //we must have a primary expression
    {
        result = expression_statement();
        //primary_expression();//FIXME this may be the incorrect action
        //expect(DELIMITER_TOKEN, SEMICOLON, NULL);
    }
    return result;

}

// <jump-statement> ::= GOTO identifier; |
//                      CONTINUE ; |
//                      BREAK ; |
//                      RETURN <expression>? ;

TOKEN jump_statement(void)
{
    TOKEN result = NULL;
    TOKEN keyword = gettok();
    if(TRUE == reserved(keyword, RETURN))
    {
        result = expression();
        result = make_return_statement(result);
    }
    expect(DELIMITER_TOKEN, SEMICOLON, NULL);
    return result;
}


// <compound-statement> ::= { <block-item-list>? }
//

TOKEN compound_statement(void)
{
    TOKEN result = NULL;
    TOKEN statements = NULL;
    startBlock();
    expect(DELIMITER_TOKEN, OPEN_BRACE, compound_statement_error_handler);
    statements = block_item_list();
    expect(DELIMITER_TOKEN, CLOSE_BRACE, NULL);
    endBlock();
    if(statements != NULL)
    {
        result = make_statement_list(statements);
    }
    
    return result;
}


// <block-item-list> ::= <block-item> <block-item-list>*
//

TOKEN block_item_list(void)
{
    TOKEN result = NULL;
    result = block_item();
    if(FALSE == delimiter(peektok(), CLOSE_BRACE))
    {
        if(result != NULL)
        {
            setLink(result, block_item_list());
        }
    }
    return result;
}

// <block-item> ::= <declaration> |
//                  <statement>

TOKEN block_item(void)
{
    TOKEN result = NULL;
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
        result = statement();
    }
    return result;
}

// <expression-statement> ::= <expression>? ;

TOKEN expression_statement(void)
{
    TOKEN exp = expression();
    expect(DELIMITER_TOKEN, SEMICOLON, NULL);
    if(exp == NULL)
    {
        exp = make_statement_list(exp);
    }
    return exp;
}


//NOTE: I am changing the grammar to dissallow single-line if statements
// <selection-statement> ::= IF ( <expression> ) <compound-statement> |
//                           IF ( <expression> ) <compound-statement> ELSE <compound-statement> | //FIXME figure out how to resolve the "else if" problem
//                           SWITCH ( <expression> ) <compound-statement>
//

TOKEN selection_statement(void)
{
    TOKEN result = NULL;
    TOKEN conditional = gettok();
    expect(DELIMITER_TOKEN, OPEN_PAREN, NULL);
    TOKEN exp = expression();
    expect(DELIMITER_TOKEN, CLOSE_PAREN, NULL);
    TOKEN actions = compound_statement();
    result = make_if(exp, actions, NULL);
    if(getWhichVal(conditional))
    {
    }
    
    return result;
}


//NOTE: I am changing the grammar to dissallow single-line loops
// <iteration-statement> ::= WHILE ( <expression> ) <compound-statement>
//                           DO <compound-statement> WHILE ( <expression> ) ;
//                           FOR ( <expression>? ; <expression>? ; <expression>? ) <compound-statement>
//                           FOR ( <declaration> <expression>? ; <expression>? ) <compound-statement>


TOKEN iteration_statement(void)
{
    TOKEN result = NULL;
    TOKEN loop_type = gettok();

    if( TRUE == reserved(loop_type, WHILE) )
    {
        expect(DELIMITER_TOKEN, OPEN_PAREN, NO_ERROR_HANDLER);
        TOKEN exp = expression();
        expect(DELIMITER_TOKEN, CLOSE_PAREN, NO_ERROR_HANDLER);
        TOKEN body = compound_statement();
        result = make_while_loop(exp, body);
    }
    else if( TRUE == reserved(loop_type, DO) )
    {
    }
    else if( TRUE == reserved(loop_type, FOR) )
    {
        expect(DELIMITER_TOKEN, OPEN_PAREN, NO_ERROR_HANDLER);
        expect(DELIMITER_TOKEN, CLOSE_PAREN, NO_ERROR_HANDLER);
    }
    else
    {
        printf("Something got screwed up while processing a loop...\n");
        exit(EXIT_FAILURE);
    }

    return result;
}

