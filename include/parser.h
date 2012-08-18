#ifndef INCLUDE_GUARD__PARSER_H_
#define INCLUDE_GUARD__PARSER_H_

#include "parse_tree.h"

TOKEN parse(void);
TOKEN translation_unit(void);
TOKEN external_declaration(void);
TOKEN function_definition(SYMBOL s);
TOKEN declaration(SYMBOL s);
TOKEN declaration_specifiers(SYMBOL s);
TOKEN init_declarator_list(SYMBOL s);
TOKEN storage_class_specifier(void);
TOKEN type_specifier(void);
TOKEN init_declarator(SYMBOL s);
TOKEN pointer(void);
TOKEN direct_declarator(SYMBOL s);
TOKEN identifier(void);
TOKEN constant(void);
TOKEN string_literal(void);
TOKEN character_constant(void);
TOKEN declarator(SYMBOL s);

TOKEN parameter_type_list(void);
TOKEN parameter_list(void);
TOKEN parameter_declaration(void);
void expect(TokenType tType, unsigned int whichToken, void (*errorAction)(void));

#endif //INCLUDE_GUARD__PARSER_H_
