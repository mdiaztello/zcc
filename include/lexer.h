#ifndef __LEX_H_
#define __LEX_H_

#include "token.h"  //for TOKEN definition


TOKEN peektok(void); //looks ahead one token into the input stream
TOKEN gettok(void); //gets tokens from the input stream mindful of whether we've peeked at tokens in the past
extern unsigned long source_code_line_number;
unsigned long getSourceLine(void);

#endif //__LEX_H_
