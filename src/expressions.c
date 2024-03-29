#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "symtab.h"
#include "token.h"
#include "token_API.h"
#include "lexer.h"
#include "parser.h"
#include "expressions.h"
#include "debug.h"
#include "error_handlers.h"



// <expression> ::= <assignment-expression> |
//                  <expression> , <assignment-expression>  FIXME: figure out how to left factor this

TOKEN expression(void)
{
    TOKEN result = NULL;
    TOKEN tok = peek_token();
    if(false == token_matches_delimiter(tok, SEMICOLON))
    {
        result = assignment_expression();
    }
    return result;
}


// <assignment-expression> ::= <conditional-expression> |  
//                             <unary-expression> <assignment-operator> <assignment-expression>

TOKEN assignment_expression(void)
{
    TOKEN result = NULL;
    result = conditional_expression();
    TOKEN tok = peek_token();
    if(result == NULL) 
    {
        result = unary_expression();
        assignment_operator();
        assignment_expression();
    }
    else
    {
        if(true == is_assignment_operator_token(tok))
        {
            TOKEN lhs = result;
            TOKEN assign = assignment_operator();
            TOKEN rhs = assignment_expression();
            result = make_binary_operation(assign, lhs, rhs);
        }
    }
    return result;
}

TOKEN assignment_operator(void)
{
    TOKEN result = NULL;
    TOKEN tok = peek_token();
    if(true == is_assignment_operator_token(tok))
    {
        result = get_token();
    }
    return result;
}

// <conditional-expression> ::= <logical-OR-expression>  //NOTE: I am explicitly disallowing use of the ternary conditional operator

TOKEN conditional_expression(void)
{
    return logical_OR_expression();
}

// <logical-OR-expression> ::= <logical-AND-expression> |
//                             <logical-AND-expression || <logical-OR-expression>

TOKEN logical_OR_expression(void)
{
    TOKEN result = NULL;
    TOKEN operand = logical_AND_expression();
    TOKEN op = NULL;
    TOKEN tok = peek_token();
    if(true == token_matches_operator(tok, BOOLEAN_OR))
    {
        op = get_token();
        result = make_binary_operation(op, operand, logical_OR_expression());
    }
    else
    {
        result = operand;
    }
    return result;
}

// <logical-AND-expression> ::= <inclusive-OR-expression> |
//                              <inclusive-OR-expression> && <logical-AND-expression>
//

TOKEN logical_AND_expression(void)
{
    TOKEN result = NULL;
    TOKEN operand = inclusive_OR_expression();
    TOKEN op = NULL;
    TOKEN tok = peek_token();
    if(true == token_matches_operator(tok, BOOLEAN_AND))
    {
        op = get_token();
        result = make_binary_operation(op, operand, logical_AND_expression());
    }
    else
    {
        result = operand;
    }
    return result;
}

// "inclusive-OR-expression" in the C standard seems to be synonymous with what I would call
// a bitwise-OR expression
// <inclusive-OR-expression> ::= <exclusive-OR-expression>  |
//                               <exclusive-OR-expression> '|' <inclusive-OR-expression>

TOKEN inclusive_OR_expression(void)
{
    TOKEN result = NULL;
    TOKEN op = NULL;
    TOKEN operand = exclusive_OR_expression();
    TOKEN tok = peek_token();
    if(true == token_matches_operator(tok, BITWISE_OR))
    {
        op = get_token();
        result = make_binary_operation(op, operand, inclusive_OR_expression());
    }
    else
    {
        result = operand;
    }
    return result;
}

// <exclusive-OR-expression> ::= <AND-expression>  |
//                               <AND-expression> ^ <exclusive-OR-expression>
//

TOKEN exclusive_OR_expression(void)
{
    TOKEN result = NULL;
    TOKEN op = NULL;
    TOKEN operand = AND_expression();
    TOKEN tok = peek_token();
    if(true == token_matches_operator(tok, BITWISE_XOR))
    {
        op = get_token();
        result = make_binary_operation(op, operand, exclusive_OR_expression());
    }
    else
    {
        result = operand;
    }

    return result;
}

// <AND-expression> ::= <equality-expression> |
//                      <equality-expression & <AND-expression>
//

TOKEN AND_expression(void)
{
    TOKEN result = NULL;
    TOKEN op = NULL;
    TOKEN operand = equality_expression();
    TOKEN tok = peek_token();
    if(true == token_matches_operator(tok, AMPERSAND))
    {
        op = get_token();
        set_token_subtype(op, BITWISE_AND);
        result = make_binary_operation(op, operand, AND_expression());
    }
    else
    {
        result = operand;
    }
    return result;
}

// <equality-expression> ::= <relational-expression> |
//                           <relational-expression> == <equality-expression> |
//                           <relational-expression> != <equality-expression>

TOKEN equality_expression(void)
{
    TOKEN result = NULL;
    TOKEN op = NULL;
    TOKEN operand = relational_expression();
    TOKEN tok = peek_token();
    if(true == token_matches_operator(tok, EQUALS) ||
            true == token_matches_operator(tok, NOT_EQUALS))
    {
        op = get_token();
        result = make_binary_operation(op, operand, equality_expression());
    }
    else
    {
        result = operand;
    }
    return result;
}

// <relational-expression> ::= <shift-expression> |
//                             <shift-expression> '>' <relational-expression>   |
//                             <shift-expression> '<' <relational-expression>   |
//                             <shift-expression> '<=' <relational-expression>  |
//                             <shift-expression> '>=' <relational-expression>

TOKEN relational_expression(void)
{
    TOKEN result = NULL;
    TOKEN op = NULL;
    TOKEN operand = shift_expression();
    TOKEN tok = peek_token();
    if(true == token_matches_operator(tok, GREATER_THAN) ||
            true == token_matches_operator(tok, GREATER_THAN_OR_EQUAL) ||
            true == token_matches_operator(tok, LESS_THAN) ||
            true == token_matches_operator(tok, LESS_THAN_OR_EQUAL) )
    {
        op = get_token();
        result = make_binary_operation(op, operand, relational_expression());
    }
    else
    {
        result = operand;
    }
    return result;
}

// <shift-expression> ::= <additive-expression>
//                        <additive-expression> << <shift-expression>
//                        <additive-expression> >> <shift-expression>

TOKEN shift_expression(void)
{
    TOKEN result = NULL;
    TOKEN op = NULL;
    TOKEN operand = additive_expression();
    TOKEN tok = peek_token();
    if(true == token_matches_operator(tok, SHIFT_LEFT) ||
            true == token_matches_operator(tok, SHIFT_RIGHT) )
    {
        op = get_token();
        result = make_binary_operation(op, operand, shift_expression());
    }
    else
    {
        result = operand;
    }
    return result;
}

// <additive-expression> ::= <multiplicative-expression> |
//                           <multiplicative-expression> + <additive-expression> |
//                           <multiplicative-expression> - <additive-expression>
//

TOKEN additive_expression(void)
{
    TOKEN op = NULL;
    TOKEN operand = NULL;
    TOKEN result = multiplicative_expression();
    TOKEN tok = peek_token();
    while(true == is_additive_operator_token(tok))
    {
        op = get_token();
        operand = multiplicative_expression();
        result = make_binary_operation(op, result, operand);
        tok = peek_token();
    }
    return result;
}


// <multiplicative-expression> ::= <cast-expression> |
//                                 <cast-expression> * <multiplicative-expression>  |
//                                 <cast-expression> / <multiplicative-expression>  |
//                                 <cast-expression> % <multiplicative-expression>

TOKEN multiplicative_expression(void)
{
    TOKEN result = cast_expression();
    TOKEN op = NULL;
    TOKEN operand = NULL;
    TOKEN tok = peek_token();
    while(true == is_multiplicative_operator_token(tok))
    {
        op = get_token();
        //we know we are parsing a multiplicative expression at this time, so we can
        //resolve STAR operators to MULTIPLICATION
        if(true == token_matches_operator(tok, STAR))
        {
            set_token_subtype(op, MULTIPLICATION);
        }
        operand = cast_expression();

        result = make_binary_operation(op, result, operand);
        tok = peek_token();
    }

    return result;
}

// <cast-expression> ::= <unary-expression> |
//                       ( <type-name> ) cast-expression

TOKEN cast_expression(void)
{
    TOKEN type_name = NULL;
    TOKEN result = NULL;
    result = unary_expression();
    if(result == NULL)
    {
        expect(DELIMITER_TOKEN, OPEN_PAREN, NULL);
        type_name = get_token();
        expect(DELIMITER_TOKEN, CLOSE_PAREN, NULL);
        result = cast_expression();
    }
    return result;
}

// <unary-expression> ::= <postfix-expression>  |
//                        ++ <unary-expression> |
//                        -- <unary-expression> |
//                        <unary-operator> <cast-expression>    |
//                        sizeof <unary-expression> |
//                        sizeof ( <type-name> )

TOKEN unary_expression(void)
{
    TOKEN result = NULL;
    TOKEN op = unary_operator();
    if( op == NULL )
    {
        result = postfix_expression();
    }
    else
    {
        TOKEN operand = cast_expression();
        result = make_unary_operation(op, operand);
    }
    return result;
}

// <unary-operator> ::= & |
//                      * |
//                      + |
//                      - |
//                      ~ |
//                      !

TOKEN unary_operator(void)
{
    TOKEN op = NULL;
    TOKEN tok = peek_token();
    if(true == is_unary_operator_token(tok))
    {
        op = get_token();
        //resolve all unary STARS and AMPERSANDS to DEREFERENCES and REFERENCES
        if(true == token_matches_operator(tok, AMPERSAND))
        {
            set_token_subtype(op, REFERENCE);
        }
        else
        {
            if(true == token_matches_operator(tok, STAR))
            {
                set_token_subtype(op, DEREFERENCE);
            }
        }
    }
    return op;
}

// <argument-expression-list> ::= <assignment-expression>   |
//                                <assignment-expression> , <argument-expression-list>

TOKEN argument_expression_list(void)
{
    TOKEN result = NULL;
    TOKEN tok = peek_token();
    if( true == token_matches_delimiter(tok, CLOSE_PAREN) )
    {
        //if this happens we are reading an empty argument list
        result = NULL;
    }
    else
    {
        result = assignment_expression();
        tok = peek_token();
        if( true == token_matches_delimiter(tok, COMMA) )
        {
            expect(DELIMITER_TOKEN, COMMA, NULL);
            set_token_link(result, argument_expression_list());
        }
    }

    return result;
}

// Since left-factoring might be painful, we just use a loop to deal with parsing the other possible productions
// <postfix-expression> ::=  <primary-expression>   |
//                           <postfix-expression> [ <expression> ]      |
//                           <postfix-expression> ( <argument-expression-list>? )       |
//                           <postfix-expression> . <identifier>        |
//                           <postfix-expression> -> <identifier>       |
//                           <postfix-expression> ++        |
//                           <postfix-expression> --        |
//                           ( <type-name> ) { <initializer-list> }     |
//                           ( <type-name> ) { <initializer-list> , }

TOKEN postfix_expression(void)
{
    TOKEN result = NULL;
    result = primary_expression();
    TOKEN tok = peek_token();
    while( true == token_matches_delimiter(tok, OPEN_BRACKET) ||
            true == token_matches_delimiter(tok, OPEN_PAREN) ||
            true == token_matches_operator(tok, DOT) ||
            true == token_matches_operator(tok, ARROW) ||
            true == token_matches_operator(tok, INCREMENT) ||
            true == token_matches_operator(tok, DECREMENT))
    {
       if(true == token_matches_delimiter(tok, OPEN_PAREN))
       {
           TOKEN function_name = NULL;
           if(result != NULL)
           {
               function_name = result;
           }
           else
           {
               printf("why is there no function name?\n");
               exit(EXIT_FAILURE);
           }
           expect(DELIMITER_TOKEN, OPEN_PAREN, NULL);
           TOKEN args = argument_expression_list();
           expect(DELIMITER_TOKEN, CLOSE_PAREN, NULL);
           result = make_function_call(function_name, args);
       }
       tok = peek_token();
    }
    return result;
}

// <primary-expression> ::= <identifier> |
//                          <constant>   |
//                          <string-literal> |
//                          ( <expression> )

//FIXME: this seems somewhat hacky...
TOKEN primary_expression(void)
{
    TOKEN result = NULL;
    TOKEN tok = peek_token();
    if(NULL == tok)
    {
        return result;
    }

    switch(get_token_type(tok))
    {
        case IDENTIFIER_TOKEN:
            result = identifier();
            break;
        case CHARACTER_LITERAL:
            result = constant();
            break;
        case NUMBER_TOKEN:
            result = constant();
            break;
        case STRING_LITERAL:
            result = string_literal();
            install_string_literal(result);
            break;
        case DELIMITER_TOKEN:
            if(true == token_matches_delimiter(tok, OPEN_PAREN))
            {
                expect(DELIMITER_TOKEN, OPEN_PAREN, NO_ERROR_HANDLER);
                result = expression();
                expect(DELIMITER_TOKEN, CLOSE_PAREN, NO_ERROR_HANDLER);
            }
            else
            {
                result = NULL;
            }
            break;
        default:
            beacon();
            printf("Why did this happen?");
            exit(0);
            break;
    }
    return result;
}
