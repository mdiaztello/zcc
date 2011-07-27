/*
 * =====================================================================================
 *
 *       Filename:  scanner.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  7/21/2011 10:24:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michael Diaz-Tello (MDT), mdiaz-tello@unvlt.com
 *        Company:  Universal Lighting Technologies
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <ctype.h>

#include "token.h"
#include "scanner.h"
#include "debug.h"
#include "global_defs.h"


int EOFFLG;

static int CHARCLASS[MAXCHARCLASS]; //character class lookup table. This is basically a lookup table based on the ASCII table
char special_characters[] = "+-*/!%&|=.,;:^<>()[]{}"; //initialize special characters lookup table

void init_scanner(void)
{
    EOFFLG = 0;
    init_charclass();
}

int peekchar(void) //peek at next character w/o moving pointer
{
    int c;
    c = getchar();
    ungetc(c,stdin);
    return c;
}

//peek at 2nd next character w/o moving pointer
int peek2char(void)
{
    int c = getchar();
    int cc = getchar();
    ungetc(cc, stdin);
    ungetc(c, stdin);
    return cc;
}

void init_charclass(void)
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
    for(i = '0'; i <= '9'; i++)
    {
        CHARCLASS[i] = NUMERIC;
    }
    for(i = '0'; special_characters[i] != '\0'; i++)
    {
        CHARCLASS[special_characters[i]] = SPECIAL;
    }

}

//determines whether the character being scanned is ALPHA (alphabetical)
//NUMBER (numeric) or SPECIAL (any of the operators, seperators, terminators, and other delimiters)
int get_char_class(char c)
{
    int cclass = CHARCLASS[c];
    return cclass;
}
