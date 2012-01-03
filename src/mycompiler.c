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
#include "scanner.h"
#include "lexer.h"
#include "parser.h"
#include "symtab.h"
#include "pprint.h"
#include "code_generator.h"

void test_lexer(void)
{
    TOKEN t;
    printf("*** TESTING THE LEXER ***\n\n");
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

void test_code_generator(void)
{
    TOKEN parse_tree = NULL;
    parse_tree = parse();
#if 0
    printf("The total number of lines of source code is %lu\n", source_code_line_number);
    printf("\n\n*** PRINTING THE SYMBOL TABLE ***\n\n");
    printst();
    printf("\n\n*** PRINTING THE PARSE TREE ***\n\n");
    ppexpr(parse_tree);
    printf("*** TESTING THE CODE GENERATOR ***\n\n");
#endif
    generate_code(parse_tree);
}

FILE* get_input_source(int argc, char** argv)
{
    FILE* input;
    if(argc > 1)
    {
        input = fopen(argv[1], "r+");
        //printf("reading input from file %s...\n", argv[1]);
    }
    else
    {
        input = stdin;
        //printf("reading input from file stdin...\n");
    }

    return input;
}

FILE* get_output_destination(int argc, char** argv)
{
    FILE* output;
    //FIXME: put in logic to parse commandline to get output
    //for now everything will go to stdout
    argc=0; //temporary to make gcc be quiet
    argv = NULL; //temporary to make gcc be quiet
    output = stdout;
    return output;
}

int main(int argc, char** argv)
{
    FILE* input = get_input_source(argc, argv);
    FILE* output = get_output_destination(argc, argv);
    init_scanner(input);
    initsyms();
    init_code_generator(output);
    //test_lexer();
    test_parser();
    //test_code_generator();
    return 0;
}
