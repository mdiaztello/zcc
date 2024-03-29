#ifndef INCLUDE_GUARD__TOKEN_API_H_
#define INCLUDE_GUARD__TOKEN_API_H_

/*
 * =====================================================================================
 *
 *       Filename:  token_API.h
 *
 *    Description:  Header file for all of the operations that can be done on token 
 *                  structures. The function prototypes are in this separate header
 *                  in order to prevent circular dependencies from forming when other
 *                  headers need access to the token struct declaration but not it's 
 *                  function prototypes.
 *
 *        Version:  1.0
 *        Created:  8/31/2011 13:52:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michael Diaz-Tello (MDT) mdiaz-tello@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdint.h>
#include <stdbool.h>
#include "symtab.h"

TOKEN make_token(void);
void set_token_type(TOKEN t, enum token_type tType);
TokenType get_token_type(TOKEN t);
void set_token_data_type(TOKEN t, DataType dType);
DataType get_token_data_type(TOKEN t);

void set_token_string_value(TOKEN t, char* string);
char* get_token_string_value(TOKEN t);

unsigned int get_token_subtype(TOKEN t);
void set_token_subtype(TOKEN t, int which);

void set_token_integer_value(TOKEN t, uint64_t intvalue);
uint64_t get_token_integer_value(TOKEN t);

TOKEN get_token_link(TOKEN tok);
void set_token_link(TOKEN tok, TOKEN linked_tok);

void set_token_operands(TOKEN tok, TOKEN operand);
TOKEN get_token_operands(TOKEN tok);

SYMBOL get_token_symbol_type(TOKEN tok);
void set_token_symbol_type(TOKEN tok, SYMBOL sym_t);

SYMBOL get_token_symbol_table_entry(TOKEN tok);
void set_token_symbol_table_entry(TOKEN tok, SYMBOL sym_entry);

double get_token_floating_point_value(TOKEN tok);
void set_token_floating_point_value(TOKEN tok, double value);

StorageClass get_token_storage_class(TOKEN tok);

bool is_keyword_token(TOKEN tok);
bool is_operator_token(TOKEN tok);
bool is_assignment_operator_token(TOKEN tok);
bool is_unary_operator_token(TOKEN tok);
bool is_multiplicative_operator_token(TOKEN tok);
bool is_additive_operator_token(TOKEN tok);
bool is_delimiter_token(TOKEN tok);

bool is_iteration_keyword_token(TOKEN tok);
bool is_jump_keyword_token(TOKEN tok);
bool is_selection_keyword_token(TOKEN tok);


bool token_matches_keyword(TOKEN tok, KeywordType desired_keyword);
bool token_matches_operator(TOKEN tok, OperatorType desired_operator);
bool token_matches_delimiter(TOKEN tok, DelimiterType desired_delimiter);

void print_token(TOKEN t);
void print_token_type(TOKEN t);
void print_token_keyword_type(TOKEN t);
void print_identifier_token(TOKEN t);
void print_token_delimiter_type(TOKEN t);
void print_token_numeric_value(TOKEN t);
void print_operator_token(TOKEN t);

#endif //INCLUDE_GUARD__TOKEN_API_H_
