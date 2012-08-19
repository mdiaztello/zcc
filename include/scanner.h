#ifndef INCLUDE_GUARD__SCANNER_H_
#define INCLUDE_GUARD__SCANNER_H_

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

//The character_type enum describes the three different types of characters
//that will appear in a stream of characters being read from a C source file.
//ALPHA characters are alphabetic (underscores are included). NUMERIC
//characters are characters in a number's representation. SPECIAL characters
//catch things like delimiters and operators.
enum character_types
{
    ALPHA = 1,
    NUMERIC = 2,
    SPECIAL = 3
};

#define DEBUG_GET_TOKEN 0

extern int EOFFLG;
char get_char(void);//get the next character from our input source, wherever that may be
char peek_char(void); //peek at next character w/o moving pointer
char peek_2_char(void); //look ahead 2 characters w/o moving the input pointer
void discard_char(void); //discards the next character in the input stream
void init_scanner(FILE* input_file); //used to point the scanner to the desired input stream
int get_char_class(char c); //used to determine what type of character we are currently looking at

#endif // INCLUDE_GUARD__SCANNER_H_
