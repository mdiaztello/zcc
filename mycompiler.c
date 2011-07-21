/*
 * =====================================================================================
 *
 *       Filename:  mycompiler.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  7/18/2011 17:15:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michael Diaz-Tello (MDT)
 *        Company:  
 *
 * =====================================================================================
 */


#include <stdio.h>
#include "token.h"
#include "lexer.h"
#include "scanner.h"

extern source_code_line_number;

void test_lexer(void)
{
    TOKEN t;
    printf("TESTING THE LEXER\n");
    while(EOFFLG == 0)
    {
        t = lex();
        if(EOFFLG == 0)
        {
            printToken(t);
        }
    }
    printf("DONE TESTING THE LEXER\n");
}

int main(void)
{
    init_scanner();
    test_lexer();
    printf("the total number of lines of source code is %d\n", source_code_line_number);
    return 0;
}
