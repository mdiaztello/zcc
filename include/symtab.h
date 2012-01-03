#ifndef __SYMTAB_H_
#define __SYMTAB_H_

/* symtab.h            Gordon S. Novak Jr.           21 Feb 07 */

/* Symbol Table Record and Definitions */

/* Copyright (c) 2007 Gordon S. Novak Jr. and
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


#include "token.h"

/* Define kinds of symbols.  The kind field should be one of these. */
#define ARGSYM       0
#define BASICTYPE    1
#define CONSTSYM     2
#define VARSYM       3
#define SUBRANGE     4
#define FUNCTIONSYM  5
#define ARRAYSYM     6   
#define RECORDSYM    7
#define TYPESYM      8
#define POINTERSYM   9

/* The following defines are commented out, but may be needed.
#define INTEGER    0
#define REAL       1
#define STRINGTYPE 2
#define BOOLETYPE  3    */

#define PPSYMDEBUG 0

#define MAXBLOCKS 50          /* Max number of program blocks           */

#define NAMESTRING_MAX_SIZE  MAX_TOKEN_STRING_LENGTH 

typedef enum storage_class
{
    UNKNOWN_STORAGE_CLASS = 0,
    TYPEDEF_STORAGE_CLASS,
    EXTERNAL_STORAGE_CLASS,
    STATIC_STORAGE_CLASS,
    AUTO_STORAGE_CLASS     //meant for temporary locals that exist only on the stack NOTE: WE ARE NOT ALLOWING USE OF THE 'auto' KEYWORD!
} StorageClass;

typedef struct symtbr 
{
    struct symtbr *link;
    char   namestring[NAMESTRING_MAX_SIZE];
    int    kind;                /* kind of symbol -- see defines below. */
    int    basicdt;             /* type code for basic data types       */
    struct symtbr *datatype;    /* pointer for more complex data types  */
    int    blocklevel;
    int    size;
    int    offset;
    StorageClass storageclass;
    union  
    { 
        char  stringconst[16];
        long  intnum;
        float realnum; 
    } constval;
    int    lowbound;
    int    highbound;
} SYMBOLREC, *SYMBOL;

void startBlock(void);
void endBlock(void);


SYMBOL symalloc(void);
int stringcmp(char s[], char t[]);
SYMBOL makesym(char name[]);
SYMBOL insertsym(char name[]);
SYMBOL searchlev(char name[], int level);
SYMBOL searchst(char name[]);
SYMBOL searchins(char name[]);
void dbprsymbol(SYMBOL sym);
void pprintsym(SYMBOL sym, int col);
void ppsym(SYMBOL sym);
void printsymbol(SYMBOL sym);
void printstlevel(int level);
void printst(void);
SYMBOL insertbt(char name[], int basictp, int siz);
SYMBOL insertfn(char name[], SYMBOL resulttp, SYMBOL argtp);
void initsyms(void);
void copy_symbol(SYMBOL original, SYMBOL destination);
void setSymbolNameString(SYMBOL sym, char* s);
void setStorageClass(SYMBOL s, StorageClass whichStorage);

int SYMTAB_DEBUG;
int blocknumber;           /* Number of current block being compiled */
int blockoffs[MAXBLOCKS];  /* Storage offsets for each block         */
int basicsizes[4];

typedef struct str_literal 
{
    struct str_literal *link;
    TOKEN string_tok;
} *STRING_LIST;

//STRING_LIST make_string_literal(char* string_name);
void install_string_literal(TOKEN string);
STRING_LIST get_string_literal_list(void);

#endif //__SYMTAB_H_
