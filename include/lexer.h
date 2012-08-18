#ifndef INCLUDE_GUARD__LEX_H_
#define INCLUDE_GUARD__LEX_H_

#include "token.h"  //for TOKEN definition
#include <stdint.h>


TOKEN peek_token(void); //looks ahead one token into the input stream
TOKEN get_token(void); //gets tokens from the input stream mindful of whether we've peeked at tokens in the past
uint64_t get_source_code_line_number(void);

#endif //INCLUDE_GUARD__LEX_H_
