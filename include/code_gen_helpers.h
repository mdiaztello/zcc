#ifndef __CODE_GEN_HELPERS_H_
#define __CODE_GEN_HELPERS_H_

/*
 * =====================================================================================
 *
 *       Filename:  code_gen_helpers.h
 *
 *    Description:  This is an internal interface that the code generator will
 *    use to interact with the platform specific portions of code generation.
 *    This interface should not be used by the outside world.
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

//the function preamble prepares the stack so the function can use it
void generate_function_preamble(void);
//the function postamble cleans up the stack after use and returns from the function
void generate_function_postamble(void);
//outputs assembly code to wherever its supposed to go
void emit(char* asm_code);
//handles assembler-specific way of telling the linker which functions are public
void register_function_name(char* function_name);
//handles the assembler-specific way of generating string literal code
void generate_string_literals(void);
void generate_function_call(TOKEN function_body);
void generate_return_code(TOKEN return_statement);

#endif
