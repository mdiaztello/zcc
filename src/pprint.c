/*  pprint.c                Gordon S. Novak Jr.              09 Feb 01  */

/*  Pretty-print a token expression tree in Lisp-like prefix form    */

/* Copyright (c) 2001 Gordon S. Novak Jr. and
   The University of Texas at Austin. */

/* This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License (file gpl.text) for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

/*  For PROGRAM, the code should look like:
     PROGRAM
       /
     GRAPH1---->PROGN---->code
                  /
               OUTPUT                  */ 

#include <ctype.h>
#include <stdio.h>
#include "token.h"
#include "token_API.h"
#include "debug.h"

#define PRINTEXPRDEBUG 0     /* Set to 1 to print each node in printexpr */
    
char* opprint[] = 
{
    "+",
    "-",
    "*",
    "/",
    "%",
    "!",
    "~",
    "&",
    "|",
    "^",
    "REFERENCE",
    "DEREFERENCE",
    ".",                    
    "=",                    
    ">",                    
    "<",                    
    "==",                   
    "!=",                   
    ">=",                   
    "<=",                   
    "||",                   
    "&&",                   
    "<<",                   
    ">>",                   
    "++",                   
    "--",                   
    "+=",                   
    "-=",                   
    "*=",                   
    "/=",                   
    "%=",                   
    "&=",                   
    "|=",                   
    "->",                   
    "STAR",                 
    "AMPERSAND",            
    "if",                   
    "progn",                
    "label",                
    "funcall",              
    "aref",                 
    "function-def",              
    "translation-unit",
    "float",                
    "return",                
    "goto",
    "fix"                   
};

//This is from Novak's code. It represents the length of each of the 
//different operators. It is needed so that the pretty-printed tree
//is aligned correctly

int opsize[] = 
{
    1,                 // "+",
    1,                 // "-",
    1,                 // "*",
    1,                 // "/",
    1,                 // "%",
    1,                 // "!",
    1,                 // "~",
    1,                 // "&",
    1,                 // "|",
    1,                 // "^",
    9,                 // "REFERENCE",
    11,                // "DEREFERENCE",
    1,                 // ".",                    
    1,                 // "=",                    
    1,                 // ">",                    
    1,                 // "<",                    
    2,                 // "==",                   
    2,                 // "!=",                   
    2,                 // ">=",                   
    2,                 // "<=",                   
    2,                 // "||",                   
    2,                 // "&&",                   
    2,                 // "<<",                   
    2,                 // ">>",                   
    2,                 // "++",                   
    2,                 // "--",                   
    2,                 // "+=",                   
    2,                 // "-=",                   
    2,                 // "*=",                   
    2,                 // "/=",                   
    2,                 // "%=",                   
    2,                 // "&=",                   
    2,                 // "|=",                   
    2,                 // "->",                   
    4,                 // "STAR",                 
    9,                 // "AMPERSAND",            
    2,                 // "if",                   
    5,                 // "progn",                
    5,                 // "label",                
    7,                 // "funcall",              
    4,                 // "aref",                 
    12,                // "function-def",              
    16,                // "translation-unit",                
    5,                 // "float",                
    6,                 // "return",                
    4,                 // "goto",                  
    3                  // "fix"                   
};

static char *delprnt[] = 
{ 
    "  ",
    ",",
    ";",
    ":",
    "(",
    ")",
    "[",
    "]",
    ".."
};

static char *resprnt[] = 
{
    " ",
    "array",
    "begin",
    "case",
    "const",
    "do",
    "downto",
    "else",
    "end",
    "file",
    "for",
    "function",
    "goto",
    "if",
    "label",
    "nil",
    "of",
    "packed",
    "procedure",
    "program",
    "record",
    "repeat",
    "set",
    "then",
    "to",
    "type",
    "until",
    "var",
    "while",
    "with" 
};

void debugprinttok(TOKEN tok)           /* print a token for debugging */
{ 
	if (tok == NULL)
	{
		printf(" token NULL%2p\n", tok);
	}
	else 
	{
		//printf( " token %6d  type %2d  which %3d  datatype %3d  symtype %6d link %6d  operands %6d\n",
				//tok, get_token_type(tok), get_token_subtype(tok), get_data_type(tok), getSymbolType(tok),
				//get_token_link(tok), getOperands(tok));
		switch (get_token_type(tok))
		{
			case DELIMITER_TOKEN:
				printf( "token %6p    type: DELIMITER_TOKEN   which: \"%s\"    datatype %3d    symtype %6p   link %6p    operands %6p\n",
						tok, delprnt[get_token_subtype(tok)], get_data_type(tok) , getSymbolType(tok),
						get_token_link(tok), getOperands(tok));
				break;
			case KEYWORD_TOKEN: 
				printf( "token %6p    type: KEYWORD_TOKEN    which: \"%s\"    datatype %3d    symtype %6p   link %6p    operands %6p\n",
						tok, resprnt[get_token_subtype(tok)], get_data_type(tok), getSymbolType(tok),
						get_token_link(tok), getOperands(tok));
				break;
            default:
                printf("I don't know what to do here\n"); beacon();
                break;
		}
	}
}

int strlength(char str[])           /* find length of a string */
{  
	int i, n;
	n = 16;
	for (i = 0; i < 16; i++)
	{
		if ( str[i] == '\0' && n == 16 ) n = i;
	}
	return n;
}

void printtok(TOKEN tok)             /* print a token in abbreviated form */
{ 
	switch (get_token_type(tok))
	{
		case IDENTIFIER_TOKEN:
			printf ("%s", get_string_value(tok));
			break;
		case STRING_LITERAL:
			printf ("'%s'", get_string_value(tok));
			break;
		case NUMBER_TOKEN:
			switch (get_data_type(tok))
			{
				case INTEGER:
					printf ("%ld", get_token_integer_value(tok));
					break;
				//case REAL:
				//	printf ("%e", getRealVal(tok));
				//	break; 
                default:
                    break;
			}
            break;
		case DELIMITER_TOKEN: 
		case KEYWORD_TOKEN: 
		case OPERATOR_TOKEN:
			break;
        default:
            break;
	}
}

void dbugprinttok(TOKEN tok)  /* print a token in 'nice' debugging form */
{ 
	if (tok == NULL)
	{
		printf(" token %6p  NULL\n", tok);
	}
	else
	{
		switch (get_token_type(tok))
		{ 
			case IDENTIFIER_TOKEN:
				printf("token %6p    ID:  \"%s\"    dtype %2d    link %6p\n",
						tok, get_string_value(tok), get_data_type(tok), get_token_link(tok));
				break;
			case STRING_LITERAL:
				printf("token %6p    STR: \"%s\"    dtype %2d    link %6p\n",
						tok, get_string_value(tok), get_data_type(tok), get_token_link(tok));
				break;
			case NUMBER_TOKEN:
				switch (get_data_type(tok))
				{
					case INTEGER:
						printf("token %6p    NUM: %12ld    dtype %2d    link %6p\n",
								tok, get_token_integer_value(tok), get_data_type(tok), get_token_link(tok));
						break;
					//case REAL:
					//	printf("token %6p    NUM: %12e    dtype %2d    link %6p\n",
					//			tok, getRealVal(tok), get_data_type(tok), get_token_link(tok));
					//	break; 
                    default:
                        break;
				};
				break;
			case OPERATOR_TOKEN:
				printf("token %6p    OP:  \"%s\"    dtype %2d    link %6p    operands %6p\n",
						tok, opprint[get_token_subtype(tok)], get_data_type(tok), get_token_link(tok),
						getOperands(tok));
				break;
			case DELIMITER_TOKEN: case KEYWORD_TOKEN:
				debugprinttok(tok);
				break;
            default:
                break;
		}
	}
}

void printexpr(TOKEN tok, int col)     /* print an expression in prefix form */
{ 
	TOKEN opnds; int nextcol, start, i;
	if(PRINTEXPRDEBUG != 0)
	{ 
		printf ("printexpr: col %d\n", col);
		dbugprinttok(tok);
	};
	if(get_token_type(tok) == OPERATOR_TOKEN)
	{ 
		printf ("(%s", opprint[get_token_subtype(tok)]);
		nextcol = col + 2 + opsize[get_token_subtype(tok)];
		opnds = getOperands(tok);
		start = 0;
		while (opnds != NULL)
		{ 
			if (start == 0) 
			{
				printf(" ");
			}
			else 
			{ 
				printf("\n");
				for (i = 0; i < nextcol; i++)
				{
					printf(" ");
				}
			}
			printexpr(opnds, nextcol);
			if ( get_token_type(opnds) == IDENTIFIER_TOKEN && nextcol < 60 )
			{
				nextcol = nextcol + 1 + strlength(get_string_value(opnds));
			}
			else
			{
				start = 1;
			}
			opnds = get_token_link(opnds);
		}
		printf (")");
	}
	else
	{
		printtok(tok);
	}
}

void ppexpr(TOKEN tok)              /* print an expression in prefix form */
{ 
	if ( tok == NULL)
	{ 
		//printf("ppexpr called with bad pointer %p\n", tok);
        printf("NULL expression found\n");
		return; 
	}

	printexpr(tok, 0);
	printf("\n");
}

void dbugplist(TOKEN tok)           /* print a list of tokens for debugging */
{ 
	while (tok != NULL)
	{ 
		dbugprinttok(tok);
		tok = get_token_link(tok);
	};
}

void dbugbprinttok(TOKEN tok)    /* print rest of token for debugging */
{ 
	if (tok != NULL)
	{
		printf("  toktype %6d  which  %6d  symtyp %6p  syment %6p  opnds %6p\n",
				get_token_type(tok), get_token_subtype(tok), getSymbolType(tok), getSymbolTableEntry(tok),
				getOperands(tok));
	}
}

void dbugprintexpr(TOKEN tok) /* print an expression in 'nice' debugging form */
{ 
	TOKEN opnds;
	dbugprinttok(tok);
	if (get_token_type(tok) == OPERATOR_TOKEN)
	{ 
		opnds = getOperands(tok);
		while (opnds != NULL)
		{ 
			dbugprintexpr(opnds);
			opnds = get_token_link(opnds);
		}
	}
}
