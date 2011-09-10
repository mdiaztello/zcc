
#include "token.h"
#include "token_API.h"
#include <stdio.h>

//contains helper functions for constructing the parse tree for the program


TOKEN make_binary_operation(TOKEN operation, TOKEN leftSide, TOKEN rightSide)
{
    setOperands(operation, leftSide);
    setLink(leftSide, rightSide);
    setLink(rightSide, NULL);
    return operation;
}


TOKEN make_unary_operation(TOKEN operation, TOKEN operand)
{
    setOperands(operation, operand);
    setLink(operand, NULL);
    return operation;
}



TOKEN make_statement_list(TOKEN statements)
{  
    TOKEN result = makeToken();
    setTokenType(result, OPERATOR_TOKEN); 
    setWhichVal(result, STATEMENT_LIST_OPERATOR);
    setOperands(result, statements);
    return result;
}

TOKEN makeif(TOKEN exp, TOKEN if_body, TOKEN else_body)
{
    TOKEN result = makeToken();
    setTokenType(result, OPERATOR_TOKEN);
    setWhichVal(result, IF_OPERATOR);
    setLink(exp, if_body);
    setLink(if_body, else_body);
    if(else_body != NULL)
    {
        setLink(else_body, NULL);
    }
    setOperands(result, exp);
    return result;
}


TOKEN make_function_call(TOKEN function_name, TOKEN args) 
{
    TOKEN function_call = makeToken();
    setTokenType(function_call, OPERATOR_TOKEN);
    setWhichVal(function_call, FUNCALL);
    setOperands(function_call, function_name);
    setLink(function_name, args);
    return function_call;
}

TOKEN make_function_definition(TOKEN function_name, TOKEN parameters, TOKEN function_body)
{
    TOKEN function_def = makeToken();
    setTokenType(function_def, OPERATOR_TOKEN);
    setWhichVal(function_def, FUNCTION_DEFINITION);
    setOperands(function_def, function_name);
    setLink(function_name, parameters);
    setLink(parameters, function_body);
    return function_def;
}

TOKEN make_return_statement(TOKEN return_exp)
{
    TOKEN ret_statement = makeToken();
    setTokenType(ret_statement, OPERATOR_TOKEN);
    setWhichVal(ret_statement, RETURN_OPERATOR);
    setOperands(ret_statement, return_exp);
    return ret_statement;
}

TOKEN make_translation_unit(TOKEN function_list)
{
    TOKEN trans_unit = makeToken();
    setTokenType(trans_unit, OPERATOR_TOKEN);
    setWhichVal(trans_unit, TRANSLATION_UNIT);
    setOperands(trans_unit, function_list);
    return trans_unit;
}

#if 0
TOKEN makeprogram(TOKEN name, TOKEN args, TOKEN statements)
{
    TOKEN program = talloc();
    program->tokentype = OPERATOR;
    program->whichval = PROGRAMOP;
    program->operands = name;
    name->link = args;
    args->link = statements;

    //dbugprinttok(name);
    //dbugprinttok(args);

    return program;
}

TOKEN makelabel(void)
{
    TOKEN label = talloc();
    TOKEN label_number = talloc();
    label->tokentype = OPERATOR;
    label->whichval = LABELOP;
    label->link = NULL;
    label->operands = label_number;
    label_number->tokentype = NUMBERTOK;
    label_number->intval = labelnum++;
    label_number->datatype = INTEGER;
    label_number->link = NULL;
    label_number->operands = NULL;
    return label;
}


TOKEN makegoto(TOKEN label)
{
    TOKEN goto_tok = talloc();
    goto_tok->tokentype = OPERATOR;
    goto_tok->whichval = GOTOOP;
    goto_tok->operands = label->operands;//copy_token(label); //FIXME: we might need to change this to search for the label in the label table instead of just making a label
    return goto_tok;
}
#endif

