#ifndef __PARSE_TREE_H_
#define __PARSE_TREE_H_



TOKEN make_binary_operation(TOKEN op, TOKEN leftSide, TOKEN rightSide);
TOKEN make_unary_operation(TOKEN operation, TOKEN operand);
TOKEN make_statement_list(TOKEN statements);
TOKEN makeif(TOKEN exp, TOKEN if_body, TOKEN else_body);
TOKEN make_function_call(TOKEN function_name, TOKEN args);
TOKEN make_function_definition(TOKEN function_name, TOKEN parameters, TOKEN function_body);
TOKEN make_translation_unit(TOKEN function_list);
TOKEN make_return_statement(TOKEN return_exp);


#endif //__PARSE_TREE_H_
