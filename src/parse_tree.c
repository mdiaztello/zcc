
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
    set_token_operands(operation, leftSide);
    set_token_link(leftSide, rightSide);
    set_token_link(rightSide, NULL);
    return operation;
}


TOKEN make_unary_operation(TOKEN operation, TOKEN operand)
{
    set_token_operands(operation, operand);
    set_token_link(operand, NULL);
    return operation;
}



TOKEN make_statement_list(TOKEN statements)
{  
    TOKEN result = make_token();
    set_token_type(result, OPERATOR_TOKEN); 
    set_token_subtype(result, PARSE_TREE_STATEMENT_LIST);
    set_token_operands(result, statements);
    return result;
}

TOKEN get_statements(TOKEN statement_list)
{
    return get_token_operands(statement_list);
}

//appends a statement to an existing statement list.
//The resulting 
TOKEN append_statement(TOKEN statement_list, TOKEN end_statement)
{
    TOKEN tail = get_token_operands(statement_list);

    while(get_token_link(tail) != NULL)
    {
        tail = get_token_link(tail);
    }
    set_token_link(tail, end_statement);
    return statement_list;
}

TOKEN make_if(TOKEN exp, TOKEN if_body, TOKEN else_body)
{
    TOKEN result = make_token();
    set_token_type(result, OPERATOR_TOKEN);
    set_token_subtype(result, PARSE_TREE_IF);
    set_token_link(exp, if_body);
    set_token_link(if_body, else_body);
    if(else_body != NULL)
    {
        set_token_link(else_body, NULL);
    }
    set_token_operands(result, exp);
    return result;
}


TOKEN make_function_call(TOKEN function_name, TOKEN args) 
{
    TOKEN function_call = make_token();
    set_token_type(function_call, OPERATOR_TOKEN);
    set_token_subtype(function_call, PARSE_TREE_FUNCALL);
    set_token_operands(function_call, function_name);
    set_token_link(function_name, args);
    return function_call;
}

TOKEN get_function_call_name(TOKEN function_call)
{
    return get_token_operands(function_call);
}

TOKEN get_function_call_args(TOKEN function_call)
{
    return get_token_link(get_function_call_name(function_call));
}

TOKEN make_function_definition(TOKEN function_name, TOKEN parameters, TOKEN function_body)
{
    TOKEN function_def = make_token();
    set_token_type(function_def, OPERATOR_TOKEN);
    set_token_subtype(function_def, PARSE_TREE_FUNCTION_DEFINITION);
    set_token_operands(function_def, function_name);
    set_token_link(function_name, parameters);
    set_token_link(parameters, function_body);
    return function_def;
}

TOKEN get_function_def_body(TOKEN function_definition)
{
    return get_token_link(get_function_def_parameters(function_definition));
}

TOKEN get_function_def_parameters(TOKEN function_definition)
{
    return get_token_link(get_function_def_name(function_definition));
}

TOKEN get_function_def_name(TOKEN function_definition)
{
    return get_token_operands(function_definition);
}

TOKEN make_return_statement(TOKEN return_exp)
{
    TOKEN ret_statement = make_token();
    set_token_type(ret_statement, OPERATOR_TOKEN);
    set_token_subtype(ret_statement, PARSE_TREE_RETURN);
    set_token_operands(ret_statement, return_exp);
    return ret_statement;
}

TOKEN make_translation_unit(TOKEN function_list)
{
    TOKEN trans_unit = make_token();
    set_token_type(trans_unit, OPERATOR_TOKEN);
    set_token_subtype(trans_unit, PARSE_TREE_TRANSLATION_UNIT);
    set_token_operands(trans_unit, function_list);
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
    TOKEN label = make_token();
    TOKEN label_number = make_token();
    set_token_type(label, OPERATOR_TOKEN);
    set_token_subtype(label, PARSE_TREE_LABEL);
    set_token_link(label, NULL);
    set_token_operands(label, label_number);
    set_token_type(label_number, NUMBER_TOKEN);
    set_token_integer_value(label_number, label_name);
    set_token_data_type(label_number, INTEGER);
    set_token_link(label_number, NULL);
    set_token_operands(label_number, NULL);
    return label;
}


TOKEN make_goto(TOKEN label)
{
    TOKEN goto_tok = make_token();
    set_token_type(goto_tok, OPERATOR_TOKEN);
    set_token_subtype(goto_tok, PARSE_TREE_GOTO);
    set_token_operands(goto_tok, get_token_operands(label));//copy_token(label); //FIXME: we might need to change this to search for the label in the label table instead of just making a label
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
    
    set_token_link(loop_start, loop_body);
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
    set_token_link(loop_start, body);
    TOKEN while_loop = make_statement_list(loop_start);

    return while_loop;
}
