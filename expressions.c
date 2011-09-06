#include "symtab.h"
#include "token.h"
#include "token_API.h"
#include "lexer.h"
#include "parser.h"
#include "expressions.h"
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"



// <expression> ::= <assignment-expression> |
//                  <expression> , <assignment-expression>  FIXME: figure out how to left factor this

void expression(void)
{
    beacon();
    TOKEN tok = peektok();
    printToken(tok);
    if(TRUE == delimiter(tok, SEMICOLON))
    {
        //we have a null statement
    }
    else
    {
        beacon();
        assignment_expression();
    }
}


// <assignment-expression> ::= <conditional-expression> |  
//                             <unary-expression> <assignment-operator> <assignment-expression>

void assignment_expression(void)
{
    //beacon();
    //unary_expression();

    //assignment_operator();
    //beacon();
    conditional_expression();
    beacon();
}

void assignment_operator(void)
{
    TOKEN tok = peektok();
    printToken(tok);
    if(FALSE == isOperator(tok))
    {
        printf("The token we saw was not an assignment operator!\n");
        exit(0);
    }
    tok = gettok();
    printToken(tok);
    switch(getWhichVal(tok))
    {
        case PLUS_EQUAL:
            break;
        case MINUS_EQUAL:
            break;
        case MULTIPLY_EQUAL:
            break;
        case DIVIDE_EQUAL:
            break;
        case MOD_EQUAL:
            break;
        case BITWISE_AND_EQUAL:
            break;
        case BITWISE_OR_EQUAL:
            break;
    }
}

// <conditional-expression> ::= <logical-OR-expression>  //NOTE: I am explicitly disallowing use of the ternary conditional operator

void conditional_expression(void)
{
    beacon();
    logical_OR_expression();
}

// <logical-OR-expression> ::= <logical-AND-expression> |
//                             <logical-AND-expression || <logical-OR-expression>

void logical_OR_expression(void)
{
    beacon();
    logical_AND_expression();
    TOKEN tok = peektok();
    if(TRUE == operator(tok, BOOLEAN_OR))
    {
        tok = gettok();
        logical_OR_expression();
    }
}

// <logical-AND-expression> ::= <inclusive-OR-expression> |
//                              <inclusive-OR-expression> && <logical-AND-expression>
//

void logical_AND_expression(void)
{
    beacon();
    inclusive_OR_expression();
    TOKEN tok = peektok();
    if(TRUE == operator(tok, BOOLEAN_AND))
    {
        tok = gettok();
        logical_AND_expression();
    }
}

// "inclusive-OR-expression" in the C standard seems to be synonymous with what I would call
// a bitwise-OR expression
// <inclusive-OR-expression> ::= <exclusive-OR-expression>  |
//                               <exclusive-OR-expression> '|' <inclusive-OR-expression>

void inclusive_OR_expression(void)
{
    beacon();
    exclusive_OR_expression();
    TOKEN tok = peektok();
    if(TRUE == operator(tok, BITWISE_OR))
    {
        tok = gettok();
        inclusive_OR_expression();
    }
}

// <exclusive-OR-expression> ::= <AND-expression>  |
//                               <AND-expression> ^ <exclusive-OR-expression>
//

void exclusive_OR_expression(void)
{
    AND_expression();
    TOKEN tok = peektok();
    if(TRUE == operator(tok, BITWISE_XOR))
    {
        tok = gettok();
        exclusive_OR_expression();
    }
}

// <AND-expression> ::= <equality-expression> |
//                      <equality-expression & <AND-expression>
//

void AND_expression(void)
{
    equality_expression();
    TOKEN tok = peektok();
    if(TRUE == operator(tok, BITWISE_AND))
    {
        tok = gettok();
        AND_expression();
    }
}

// <equality-expression> ::= <relational-expression> |
//                           <relational-expression> == <equality-expression> |
//                           <relational-expression> != <equality-expression>

void equality_expression(void)
{
    beacon();
    relational_expression();
    TOKEN tok = peektok();
    if(TRUE == operator(tok, EQUALS) ||
            TRUE == operator(tok, NOT_EQUALS))
    {
        tok = gettok();
        equality_expression();
    }
    
}

// <relational-expression> ::= <shift-expression> |
//                             <shift-expression> '>' <relational-expression>   |
//                             <shift-expression> '<' <relational-expression>   |
//                             <shift-expression> '<=' <relational-expression>  |
//                             <shift-expression> '>=' <relational-expression>

void relational_expression(void)
{
    shift_expression();
    TOKEN tok = peektok();
    if(TRUE == operator(tok, GREATER_THAN) ||
            TRUE == operator(tok, GREATER_THAN_OR_EQUAL) ||
            TRUE == operator(tok, LESS_THAN) ||
            TRUE == operator(tok, LESS_THAN_OR_EQUAL) )
    {
        tok = gettok();
        relational_expression();
    }
}

// <shift-expression> ::= <additive-expression>
//                        <additive-expression> << <shift-expression>
//                        <additive-expression> >> <shift-expression>

void shift_expression(void)
{
    additive_expression();
    TOKEN tok = peektok();
    if(TRUE == operator(tok, SHIFT_LEFT) ||
            TRUE == operator(tok, SHIFT_RIGHT) )
    {
        tok = gettok();
        shift_expression();
    }

}

// <additive-expression> ::= <multiplicative-expression> |
//                           <multiplicative-expression> + <additive-expression> |
//                           <multiplicative-expression> - <additive-expression>
//

void additive_expression(void)
{
    multiplicative_expression();
    TOKEN tok = peektok();
    if(TRUE == operator(tok, ADDITION) ||
            TRUE == operator(tok, SUBTRACTION) )
    {
        tok = gettok();
        additive_expression();
    }

}


// <multiplicative-expression> ::= <cast-expression> |
//                                 <cast-expression> * <multiplicative-expression>  |
//                                 <cast-expression> / <multiplicative-expression>  |
//                                 <cast-expression> % <multiplicative-expression>

void multiplicative_expression(void)
{
    cast_expression();
    TOKEN tok = peektok();
    if(TRUE == operator(tok, MULTIPLICATION) ||
            TRUE == operator(tok, DIVISION) ||
            TRUE == operator(tok, MODULAR_DIVISION) )
    {
        tok = gettok();
        multiplicative_expression();
    }
}

// <cast-expression> ::= <unary-expression> |
//                       ( <type-name> ) cast-expression

void cast_expression(void)
{
    beacon(); 
    primary_expression();
}

// <unary-expression> ::= <postfix-expression>  |
//                        ++ <unary-expression> |
//                        -- <unary-expression> |
//                        <unary-operator> <cast-expression>    |
//                        sizeof <unary-expression> |
//                        sizeof ( <type-name> )

void unary_expression(void)
{
    printToken(peektok());
    postfix_expression();
}

// <unary-operator> ::= & |
//                      * |
//                      + |
//                      - |
//                      ~ |
//                      !

// <argument-expression-list> ::= <assignment-expression>   |
//                                <assignment-expression> , <argument-expression-list>


//FIXME: find a way to left factor this production
// <postfix-expression> ::=  <primary-expression>   |
//                           <postfix-expression> [ <expression> ]      |
//                           <postfix-expression> ( <argument-expression-list>? )       |
//                           <postfix-expression> . <identifier>        |
//                           <postfix-expression> -> <identifier>       |
//                           <postfix-expression> ++        |
//                           <postfix-expression> --        |
//                           ( <type-name> ) { <initializer-list> }     |
//                           ( <type-name> ) { <initializer-list> , }

void postfix_expression(void)
{
    printToken(peektok());
    primary_expression();
}


// <primary-expression> ::= <identifier> |
//                          <constant>   |
//                          <string-literal> |
//                          ( <expression> )

void primary_expression(void)
{
    TOKEN tok = peektok();
    if(TRUE == delimiter(tok, OPEN_PAREN))
    {
        expect(DELIMITER_TOKEN, OPEN_PAREN, NULL);
        beacon();
        expression();
        expect(DELIMITER_TOKEN, CLOSE_PAREN, NULL);
    }
    else
    {
        identifier();
    }
}
