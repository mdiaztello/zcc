#include <stdio.h>
#include "token_API.h"
#include "symtab.h"
#include "parse_tree.h"
#include "code_gen_helpers.h"

/*
 * =====================================================================================
 *
 *       Filename:  code_gen_x86_64.c
 *
 *    Description:  This is the platform-specific portion of the code generator that
 *    will output code for x86_64 using GCC for assembly and linking and (implicitly)
 *    running on a Linux system.
 *
 *        Version:  1.0
 *        Created:  2011/12/30 17:15:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michael Diaz-Tello (MDT) mdiaz-tello@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

void generate_function_preamble(void)
{
    emit("\t# **** FUNCTION PREAMBLE ****\n");
    emit("\tpushq %rbp          #set up stack frame\n");
	emit("\tmovq %rsp, %rbp     #set up stack frame\n");
}

void generate_function_postamble(void)
{
    emit("\n\t# **** FUNCTION POSTAMBLE ****\n");
    emit("\tleave   #restore stack frame\n");
	emit("\tret     #return from function call\n");
}


void register_function_name(char* function_name)
{
    emit("\n.global ");
    emit(function_name);
    emit("\n");
}

//FIXME this works but I may want to refine it...
void generate_string_literals(void)
{
    STRING_LIST string_literal_list = get_string_literal_list();
    emit(".section .rodata\n");
    while(string_literal_list != NULL)
    {
        emit(getSymbolTableEntry(string_literal_list->string_tok)->namestring);
        emit(": .string \"");
        emit(getStringVal(string_literal_list->string_tok));
        emit("\"\n");
        string_literal_list = string_literal_list->link;
    }
    emit("\n.text  #beginning of the code\n");
}

//FIXME FILTHY HACK
void generate_function_call(TOKEN function_call)
{
    TOKEN func_name = get_function_call_name(function_call);
    TOKEN arg = get_function_call_args(function_call);
    emit("\tmovl $");
    emit(getSymbolTableEntry(arg)->namestring);
    emit(", %edi\n");
    emit("\tcall ");
    emit(getStringVal(func_name));
    emit("\n");
}

//FIXME FILTHY HACK
void generate_return_code(TOKEN return_statement)
{
    TOKEN return_val = getOperands(return_statement);
    emit("\tmovl $");
    char value[10];
    sprintf(value, "%lu", getIntegerValue(return_val));
    emit(value);
    emit(", %eax\n");
}
