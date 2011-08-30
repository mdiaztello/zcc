#ifndef __SCANNER_H_
#define __SCANNER_H_

/*
 * =====================================================================================
 *
 *       Filename:  scanner.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  7/21/2011 10:38:37
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michael Diaz-Tello (MDT), mdiaz-tello@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#define MAXCHARCLASS 256

enum character_types
{
    ALPHA = 1,
    NUMERIC = 2,
    SPECIAL = 3
};

#define DEBUG_GET_TOKEN 0

int EOFFLG;
char peekchar(void); //peek at next character w/o moving pointer
char peek2char(void);
void discard_char(void);
void init_charclass(void);
void init_scanner(void);
int get_char_class(char c);

#endif // __SCANNER_H_
