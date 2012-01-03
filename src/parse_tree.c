
#include "token.h"
#include "token_API.h"
#include "symtab.h"
#include <stdio.h>
#include "debug.h"
#include "parse_tree.h"
#include <stdint.h>

//contains helper functions for constructing the parse tree for the program


static uint64_t labelnum = 0;

uint64_t get_new_label_number(void)
{
    labelnum++;
    return labelnum;
}

//returns the name of the most recent label
uint64_t get_current_label(void)
{
    return labelnum;
}

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
    setWhichVal(result, PARSE_TREE_STATEMENT_LIST);
    setOperands(result, statements);
    return result;
}

TOKEN get_statements(TOKEN statement_list)
{
    return getOperands(statement_list);
}

//appends a statement to an existing statement list.
//The resulting 
TOKEN append_statement(TOKEN statement_list, TOKEN end_statement)
{
    TOKEN tail = getOperands(statement_list);

    while(getLink(tail) != NULL)
    {
        tail = getLink(tail);
    }
    setLink(tail, end_statement);
    return statement_list;
}

TOKEN make_if(TOKEN exp, TOKEN if_body, TOKEN else_body)
{
    TOKEN result = makeToken();
    setTokenType(result, OPERATOR_TOKEN);
    setWhichVal(result, PARSE_TREE_IF);
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
    setWhichVal(function_call, PARSE_TREE_FUNCALL);
    setOperands(function_call, function_name);
    setLink(function_name, args);
    return function_call;
}

TOKEN get_function_call_name(TOKEN function_call)
{
    return getOperands(function_call);
}

TOKEN get_function_call_args(TOKEN function_call)
{
    return getLink(get_function_call_name(function_call));
}

TOKEN make_function_definition(TOKEN function_name, TOKEN parameters, TOKEN function_body)
{
    TOKEN function_def = makeToken();
    setTokenType(function_def, OPERATOR_TOKEN);
    setWhichVal(function_def, PARSE_TREE_FUNCTION_DEFINITION);
    setOperands(function_def, function_name);
    setLink(function_name, parameters);
    setLink(parameters, function_body);
    return function_def;
}

TOKEN get_function_def_body(TOKEN function_definition)
{
    return getLink(get_function_def_parameters(function_definition));
}

TOKEN get_function_def_parameters(TOKEN function_definition)
{
    return getLink(get_function_def_name(function_definition));
}

TOKEN get_function_def_name(TOKEN function_definition)
{
    return getOperands(function_definition);
}

TOKEN make_return_statement(TOKEN return_exp)
{
    TOKEN ret_statement = makeToken();
    setTokenType(ret_statement, OPERATOR_TOKEN);
    setWhichVal(ret_statement, PARSE_TREE_RETURN);
    setOperands(ret_statement, return_exp);
    return ret_statement;
}

TOKEN make_translation_unit(TOKEN function_list)
{
    TOKEN trans_unit = makeToken();
    setTokenType(trans_unit, OPERATOR_TOKEN);
    setWhichVal(trans_unit, PARSE_TREE_TRANSLATION_UNIT);
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
#endif

TOKEN make_label(uint64_t label_name)
{
    TOKEN label = makeToken();
    TOKEN label_number = makeToken();
    setTokenType(label, OPERATOR_TOKEN);
    setWhichVal(label, PARSE_TREE_LABEL);
    setLink(label, NULL);
    setOperands(label, label_number);
    setTokenType(label_number, NUMBER_TOKEN);
    setIntegerValue(label_number, label_name);
    setDataType(label_number, INTEGER);
    setLink(label_number, NULL);
    setOperands(label_number, NULL);
    return label;
}


TOKEN make_goto(TOKEN label)
{
    TOKEN goto_tok = makeToken();
    setTokenType(goto_tok, OPERATOR_TOKEN);
    setWhichVal(goto_tok, PARSE_TREE_GOTO);
    setOperands(goto_tok, getOperands(label));//copy_token(label); //FIXME: we might need to change this to search for the label in the label table instead of just making a label
    return goto_tok;
}


//Output takes the form
//      (PROGN (LABEL X)
//             (IF EXP (PROGN BODY (GOTO X))))

TOKEN make_while_loop(TOKEN exp, TOKEN body)
{
    TOKEN loop_start = make_label(get_new_label_number());
    TOKEN loop_branch = make_goto(loop_start);
    body = append_statement(body, loop_branch);
    TOKEN loop_body = make_if(exp, body, NULL);
    
    setLink(loop_start, loop_body);
    TOKEN while_loop = make_statement_list(loop_start);

    return while_loop;
}

//Output takes the form
//         (PROGN (LABEL X)
//                (BODY)
//                (IF EXP (GOTO X))

TOKEN make_do_loop(TOKEN exp, TOKEN body)
{
    TOKEN loop_start = make_label(get_new_label_number());
    TOKEN loop_branch = make_goto(loop_start);
    TOKEN loop_test = make_if(exp, loop_branch, NULL);
    
    body = append_statement(body, loop_test);
    setLink(loop_start, body);
    TOKEN while_loop = make_statement_list(loop_start);

    return while_loop;
}
