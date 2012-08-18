#ifndef INCLUDE_GUARD__CODE_GEN_H_
#define INCLUDE_GUARD__CODE_GEN_H_

/*
 * =====================================================================================
 *
 *       Filename:  code_generator.h
 *
 *    Description:  This is the outward-facing interface that the rest of the compiler
 *    will invoke when it wants to do code generation.
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

void init_code_generator(FILE* output_file);
void generate_code(TOKEN parse_tree);

#endif
