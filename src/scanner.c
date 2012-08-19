/*
 * =====================================================================================
 *
 *       Filename:  scanner.c
 *
 *    Description:  This file handles all of the low level scanning routines that pass
 *    characters from the file being inspected to the lexical analyser for the creation
 *    of tokens.
 *
 *        Version:  1.0
 *        Created:  7/21/2011 10:24:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michael Diaz-Tello (MDT), mdiaz-tello@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <ctype.h>
#include "token.h"
#include "scanner.h"
#include "debug.h"


int EOFFLG;

static void init_charclass(void);

#define MAXCHARCLASS 256

static int CHARCLASS[MAXCHARCLASS]; //character class lookup table. This is basically a lookup table based on the ASCII table
char special_characters[] = "+-*/!%&|~^=.,;:<>()[]{}"; //initialize special characters lookup table

//represents the source of the incoming code
//either a file or stdin 
//FIXME: I may want to have support for reading
//directly from a character buffer in the future
static FILE* input_source; 

void init_scanner(FILE* input_file)
{
    EOFFLG = 0;
    init_charclass();
    input_source = input_file;
}

//get the next character from our input source, whatever it may be
char get_char(void)
{
    return fgetc(input_source);
}

char peek_char(void) //peek at next character w/o moving pointer
{
    char c;
    c = get_char();
    ungetc(c,input_source);
    return c;
}

//peek at 2nd next character w/o moving pointer
char peek_2_char(void)
{
    char c = get_char();
    char cc = get_char();
    ungetc(cc, input_source);
    ungetc(c, input_source);
    return cc;
}

//throw away the next character from the input source
//this is used to clarify intent
void discard_char(void)
{
    (void) get_char();
}

//initializes our character class lookup table, which helps us determine
//what kind of character we've received from the input
static void init_charclass(void)
{
    int i;
    for(i = 0; i < MAXCHARCLASS; i++)
    {
        CHARCLASS[i] = 0;
    }
    for(i = 'A'; i <= 'Z'; i++)
    {
        CHARCLASS[i] = ALPHA;
    }
    for(i = 'a'; i <= 'z'; i++)
    {
        CHARCLASS[i] = ALPHA;
    }

    CHARCLASS['_'] = ALPHA; //underscores considered alphanumeric for the purposes of identifier names

    for(i = '0'; i <= '9'; i++)
    {
        CHARCLASS[i] = NUMERIC;
    }
    for(i = 0; special_characters[i] != '\0'; i++)
    {
        CHARCLASS[(size_t)special_characters[i]] = SPECIAL;
    }
}

//determines whether the character being scanned is ALPHA (alphabetical)
//NUMBER (numeric) or SPECIAL (any of the operators, seperators, terminators, and other delimiters)
int get_char_class(char c)
{
    int cclass = CHARCLASS[(size_t)c];
    return cclass;
}
