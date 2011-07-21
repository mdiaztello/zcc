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
 *         Author:  Michael Diaz-Tello (MDT), mdiaz-tello@unvlt.com
 *        Company:  Universal Lighting Technologies
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
int CHARCLASS[MAXCHARCLASS]; //character class lookup table 
int peekchar(void); //peek at next character w/o moving pointer
int peek2char(void);
void init_charclass(void);
void init_scanner(void);
