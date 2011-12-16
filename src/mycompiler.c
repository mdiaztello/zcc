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
 *         Author:  Michael Diaz-Tello (MDT) mdiaz-tello@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "token.h"
#include "token_API.h"
#include "lexer.h"
#include "scanner.h"
#include "parser.h"
#include "symtab.h"
#include "pprint.h"


void test_lexer(void)
{
    TOKEN t;
    printf("*** TESTING THE LEXER ***\n\n");
#if 0
    while(EOFFLG == 0)
    {
        for(int i = 0; i< 10; i++)
        {
            t = peektok();
            if(EOFFLG == 0)
            {
                printToken(t);
            }
            t = peektok();
            if(EOFFLG == 0)
            {
                printToken(t);
            }
            t = gettok();
            if(EOFFLG == 0)
            {
                printToken(t);
            }
        }
        break;
    }
#endif

    while( (t = gettok()) != NULL )
    {
        printToken(t);
    }


    printf("\n*** DONE TESTING THE LEXER ***\n");
}


void test_parser(void)
{
    TOKEN parse_tree = NULL;
    printf("*** TESTING THE PARSER ***\n\n");
    parse_tree = parse();
    printf("The total number of lines of source code is %lu\n", source_code_line_number);
    printf("\n*** DONE TESTING THE PARSER ***\n\n");
    printf("\n\n*** PRINTING THE SYMBOL TABLE ***\n\n");
    printst();
    printf("\n\n*** PRINTING THE PARSE TREE ***\n\n");
    ppexpr(parse_tree);
}

FILE* get_input_source(int argc, char** argv)
{
    FILE* input;
    if(argc > 1)
    {
        input = fopen(argv[1], "r+");
        printf("reading input from file %s...\n", argv[1]);
    }
    else
    {
        input = stdin;
        printf("reading input from file stdin...\n");
    }

    return input;
}


int main(int argc, char** argv)
{
    FILE* input = get_input_source(argc, argv);
    init_scanner(input);
    initsyms();
    //test_lexer();
    test_parser();
    return 0;
}
