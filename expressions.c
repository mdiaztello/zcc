#include <stdio.h>
#include <stdlib.h>
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
    TOKEN tok = peektok();
    if(FALSE == delimiter(tok, SEMICOLON))
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
    TOKEN tok = peektok();
    if(result == NULL) 
    {
        result = unary_expression();
        assignment_operator();
        assignment_expression();
    }
    else
    {
        if(TRUE == isAssignmentOperator(tok))
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
    TOKEN tok = peektok();
    if(TRUE == isAssignmentOperator(tok))
    {
        result = gettok();
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
    TOKEN tok = peektok();
    if(TRUE == operator(tok, BOOLEAN_OR))
    {
        op = gettok();
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
    TOKEN tok = peektok();
    if(TRUE == operator(tok, BOOLEAN_AND))
    {
        op = gettok();
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
    TOKEN tok = peektok();
    if(TRUE == operator(tok, BITWISE_OR))
    {
        op = gettok();
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
    TOKEN tok = peektok();
    if(TRUE == operator(tok, BITWISE_XOR))
    {
        op = gettok();
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
    TOKEN tok = peektok();
    if(TRUE == operator(tok, AMPERSAND))
    {
        op = gettok();
        setWhichVal(op, BITWISE_AND);
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
    TOKEN tok = peektok();
    if(TRUE == operator(tok, EQUALS) ||
            TRUE == operator(tok, NOT_EQUALS))
    {
        op = gettok();
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
    TOKEN tok = peektok();
    if(TRUE == operator(tok, GREATER_THAN) ||
            TRUE == operator(tok, GREATER_THAN_OR_EQUAL) ||
            TRUE == operator(tok, LESS_THAN) ||
            TRUE == operator(tok, LESS_THAN_OR_EQUAL) )
    {
        op = gettok();
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
    TOKEN tok = peektok();
    if(TRUE == operator(tok, SHIFT_LEFT) ||
            TRUE == operator(tok, SHIFT_RIGHT) )
    {
        op = gettok();
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
    TOKEN tok = peektok();
    while(TRUE == isAdditiveOperator(tok))
    {
        op = gettok();
        operand = multiplicative_expression();
        result = make_binary_operation(op, result, operand);
        tok = peektok();
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
    TOKEN tok = peektok();
    while(TRUE == isMultiplicativeOperator(tok))
    {
        op = gettok();
        //we know we are parsing a multiplicative expression at this time, so we can
        //resolve STAR operators to MULTIPLICATION
        if(TRUE == operator(tok, STAR))
        {
            setWhichVal(op, MULTIPLICATION);
        }
        operand = cast_expression();

        result = make_binary_operation(op, result, operand);
        tok = peektok();
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
        type_name = gettok();
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
    TOKEN tok = peektok();
    if(TRUE == isUnaryOperator(tok))
    {
        op = gettok();
        //resolve all unary STARS and AMPERSANDS to DEREFERENCES and REFERENCES
        if(TRUE == operator(tok, AMPERSAND))
        {
            setWhichVal(op, REFERENCE);
        }
        else
        {
            if(TRUE == operator(tok, STAR))
            {
                setWhichVal(op, DEREFERENCE);
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
    TOKEN tok = peektok();
    if( TRUE == delimiter(tok, CLOSE_PAREN) )
    {
        //if this happens we are reading an empty argument list
        result = NULL;
    }
    else
    {
        result = assignment_expression();
        tok = peektok();
        if( TRUE == delimiter(tok, COMMA) )
        {
            expect(DELIMITER_TOKEN, COMMA, NULL);
            setLink(result, argument_expression_list());
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
    TOKEN tok = peektok();
    while( TRUE == delimiter(tok, OPEN_BRACKET) ||
            TRUE == delimiter(tok, OPEN_PAREN) ||
            TRUE == operator(tok, DOT) ||
            TRUE == operator(tok, ARROW) ||
            TRUE == operator(tok, INCREMENT) ||
            TRUE == operator(tok, DECREMENT))
    {
       if(TRUE == delimiter(tok, OPEN_PAREN))
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
       tok = peektok();
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
    TOKEN tok = peektok();

    switch(getTokenType(tok))
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
            break;
        case DELIMITER_TOKEN:
            if(TRUE == delimiter(tok, OPEN_PAREN))
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
