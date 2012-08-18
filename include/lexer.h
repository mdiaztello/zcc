#ifndef __LEX_H_
#define __LEX_H_

#include "token.h"  //for TOKEN definition
#include <stdint.h>


TOKEN peektok(void); //looks ahead one token into the input stream
TOKEN gettok(void); //gets tokens from the input stream mindful of whether we've peeked at tokens in the past
uint64_t get_source_code_line_number(void);

#endif //__LEX_H_
