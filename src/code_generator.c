#include <stdio.h>
#include "token.h"
#include "token_API.h"
#include "parse_tree.h"
#include "code_generator.h"
#include "code_gen_helpers.h"
#include "pprint.h"

//represents where the asm output will go
//could be either stdin or a file
//FIXME: I may want to store this locally so I can
//optimize directly from memory instead of having to
//re-read a text file
FILE* output;

static void generate_function(TOKEN parse_tree);
static void generate_function_label(TOKEN function_definition);
static void generate_function_body(TOKEN function_definition);

void init_code_generator(FILE* output_file)
{
    output = output_file;
}

void generate_code(TOKEN parse_tree)
{
    if(NULL == parse_tree)
    {
        return;
    }
    generate_string_literals();
    TOKEN function_definition = getOperands(parse_tree);
    while(NULL != function_definition)
    {
        generate_function(function_definition);
        function_definition = getLink(function_definition);
    }
}

//This function encapsulates where the output should go, so the platform
//specific code can just worry about generating the low-level code
void emit(char* asm_code)
{
    fprintf(output, "%s", asm_code);
}

static void generate_function(TOKEN function_definition)
{
    generate_function_label(function_definition);
    generate_function_preamble();
    generate_function_body(function_definition);
    generate_function_postamble();
}

void generate_function_label(TOKEN function_definition)
{
    TOKEN function_name = get_function_def_name(function_definition);
    register_function_name(getStringVal(function_name));
    emit("\n");
    emit(getStringVal(function_name));
    emit(":\n");
}

//FIXME
void generate_function_body(TOKEN function_definition)
{
    TOKEN function_body = get_statements(get_function_def_body(function_definition));
    emit("\n\t# **** FUNCTION BODY ****\n\n");
    while(NULL != function_body)
    {
        if(TRUE == _operator(function_body, PARSE_TREE_FUNCALL))
        {
            generate_function_call(function_body);
        }
        if(TRUE == _operator(function_body, PARSE_TREE_RETURN))
        {
            generate_return_code(function_body);
        }
        function_body = getLink(function_body);
    }
}
