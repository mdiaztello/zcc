/* symtab.c            Gordon S. Novak Jr.             06 Aug 09    */

/* Symbol Table Code and Data for Pascal Compiler */
/* See the documentation file, symtab.txt         */

/* Copyright (c) 2007 Gordon S. Novak Jr. and
   The University of Texas at Austin. */

/* 09 Feb 01; 21 Feb 07 */

/* This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License (file gpl.text) for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  */


/* To use:  1. Call initsyms() once to initialize basic symbols.

            2. Call insertsym(string) to insert a new symbol,
                e.g. insertsym(tok->stringval);
                returns a symbol table pointer, of type SYMBOL.

               makesym and symalloc will also be used.

	    3. Call searchst(string) to search for a symbol,
                e.g. searchst(tok->stringval);
                returns a symbol table pointer, of type SYMBOL.
  */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "token.h"
#include "token_API.h"
#include "debug.h"


/* BASEOFFSET is the offset for the first variable */
#define BASEOFFSET 8

int    blocknumber = 0;       /* Number of current block being compiled */
int    contblock[MAXBLOCKS];  /* Containing block for each block        */
int    blockoffs[MAXBLOCKS];  /* Storage offsets for each block         */
SYMBOL symtab[MAXBLOCKS];     /* Symbol chain for each block            */
SYMBOL symend[MAXBLOCKS];     /* End of symbol chain for each block     */

int    basicsizes[4] = { 4, 8, 1, 4 };  /* Sizes of basic types */

char* symprint[10]  = {" ", "BASIC", "CONST", "VAR", "SUBRANGE",
                       "FUNCTION", "ARRAY", "RECORD", "TYPE", "POINTER"};
int symsize[10] = { 1, 5, 5, 3, 8, 8, 5, 6, 4, 7 };


//every time we begin a new lexical block, its symbols should go in a new block in the symbol table
void startBlock(void)
{
    blocknumber++;
}

//every time we reach the end of a lexical block, return to the previous block level
void endBlock(void)
{
    blocknumber--;
}

int getBlockLevel(void)
{
    return blocknumber;
}


void setSymbolNameString(SYMBOL sym, char* s)
{
    int i = 0;

    for(i = 0; (i <NAMESTRING_MAX_SIZE  ) && (s[i] != 0); i++)
    {
        sym->namestring[i] = s[i];
    }
    sym->namestring[i] = 0; //terminate the string FIXME: there may be an off-by-one buffer overflow here...
}

SYMBOL symalloc(void)           /* allocate a new symbol record */
{ 
    return((SYMBOL) calloc(1,sizeof(SYMBOLREC)));
}

/* Make a symbol table entry with the given name */
SYMBOL makesym(char name[])
{ 
    SYMBOL sym; 
    int i;
    sym = symalloc();
    for ( i = 0; i < 16; i++)
    {
        sym->namestring[i] = name[i];
    }
    sym->link = NULL;
    return sym;
}

/* Insert a name in the symbol table at current value of blocknumber */
/* Returns pointer to the new symbol table entry, which is empty     */
/* except for the name.                                              */
SYMBOL insertsym(char name[])
{ 
    SYMBOL sym;
    sym = makesym(name);
    if ( symtab[blocknumber] == NULL )  /* Insert in 2-pointer queue */
    {
        symtab[blocknumber] = sym;
    }
    else
    {
        symend[blocknumber]->link = sym;
    }
    symend[blocknumber] = sym;
    sym->blocklevel = blocknumber;
    if (SYMTAB_DEBUG)
    {
        printf("insertsym %8s %6p\n", name, sym);
    }
    return sym;
}

/* Search one level of the symbol table for the given name.         */
/* Result is a pointer to the symbol table entry or NULL            */
SYMBOL searchlev(char name[], int level)
{ 
    SYMBOL sym;
    sym = symtab[level];
    while ( sym != NULL && strcmp(name, sym->namestring) != 0 )
    {
        sym = sym->link;
    }
    return sym;
}

/* Search all levels of the symbol table for the given name.        */
/* Result is a pointer to the symbol table entry or NULL            */
SYMBOL searchst(char name[])
{ 
    SYMBOL sym; 
    int level;
    level = blocknumber;
    sym = NULL;
    while ( sym == NULL && level >= 0 )
    {  
        sym = searchlev(name, level);
        if (level > 0)
        {
            level = contblock[level]; /* try containing block */
        }
        else
        {
            level = -1;                      /* until all are tried  */
        }
    }
    if(SYMTAB_DEBUG)
    {
        printf("searchst  %8s %6p\n", name, sym);
    }
    return sym;
}

/* Search for symbol, insert if not there. */
SYMBOL searchins(char name[])
{  
    SYMBOL res;
    res = searchst(name);
    if ( res != NULL )
    {
        return(res);
    }
    res = insertsym(name);
    return(res);
}

/* Print one symbol table entry for debugging      */
void dbprsymbol(SYMBOL sym)
{ 
    if( sym != NULL )
    {
        printf(" %6p  %10s  knd %1d  typ %7p  lvl %2d  siz %5d  off %5d lnk %5p\n",
                sym, sym->namestring, sym->kind, sym->datatype,
                sym->blocklevel, sym->size, sym->offset, sym->link);
    }
}

void pprintsym(SYMBOL sym, int col)   /* print type expression in prefix form */
{ 
    SYMBOL opnds; 
    int nextcol, start, done, i;
    if (PPSYMDEBUG != 0)
    { 
        printf ("pprintsym: col %d\n", col);
        dbprsymbol(sym);
    };
    switch (sym->kind)
    { 
        case BASICTYPE:
            printf("%s", sym->namestring);
            nextcol = col + 1 + strlen(sym->namestring);
            break;
        case SUBRANGE:
            printf("%3d ..%4d", sym->lowbound, sym->highbound);
            nextcol = col + 10;
            break;
        case POINTERSYM:
            if (sym->datatype->namestring != 0)
            {
                printf("(^ %s)", sym->datatype->namestring);
            }
            else
            {
                printf("(^ %7p)", sym->datatype);
            }
            break;
        case FUNCTIONSYM:
        case ARRAYSYM:
        case RECORDSYM:
            printf ("(%s", symprint[sym->kind]);
            nextcol = col + 2 + symsize[sym->kind];
            if( sym->kind == ARRAYSYM )
            {  
                printf(" %3d ..%4d", sym->lowbound, sym->highbound);
                nextcol = nextcol + 11;
            }
            opnds = sym->datatype;
            start = 0;
            done = 0;
            while ( opnds != NULL && done == 0 )
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
                };
                if ( sym->kind == RECORDSYM )
                {  
                    printf("(%s ", opnds->namestring);
                    pprintsym(opnds, nextcol + 2
                            + strlen(opnds->namestring));
                    printf(")");
                }
                else
                {
                    pprintsym(opnds, nextcol);
                }
                start = 1;
                if ( sym->kind == ARRAYSYM )
                {
                    done = 1;
                }
                opnds = opnds->link;
            }
            printf(")");
            break;
        default:
            if ( sym->datatype != NULL) 
            {
                pprintsym(sym->datatype, col);
            }
            else
            {
                printf("NULL");
            }
            break;
    }
}

void ppsym(SYMBOL sym)             /* print a type expression in prefix form */
{ 
    pprintsym(sym, 0);
    printf("\n");
}

char* printStorageClass(SYMBOL sym)
{
    char* s;
    switch(sym->storageclass)
    {
        case UNKNOWN_STORAGE_CLASS:
            s = "UNKNOWN";
            break;
        case TYPEDEF_STORAGE_CLASS:
            s = "TYPEDEF";
            break;
        case EXTERNAL_STORAGE_CLASS:
            s = "EXTERN";
            break;
        case STATIC_STORAGE_CLASS:
            s = "STATIC";
            break;
        case AUTO_STORAGE_CLASS:
            s = "AUTO";
            break;
    }
    return s;
}



/* Print one symbol table entry       */
void printsymbol(SYMBOL sym)
{  
    if(sym == NULL)
    {
        printf("CAN'T PRINT NULL SYMBOLS!\n");
    }
    switch (sym->kind)
    { 
        case FUNCTIONSYM: 
        case ARRAYSYM:
        case RECORDSYM: 
        case POINTERSYM:
            printf(" %6p  %10s  knd %1d  typ %7p  lvl %2d  siz %5d  off %5d\n",
                    sym, sym->namestring, sym->kind, sym->datatype,
                    sym->blocklevel, sym->size, sym->offset);
            ppsym(sym);
            break;
        case VARSYM:
            if (sym->datatype->kind == BASICTYPE)
            {
                
                printf(" %6p  %10s  VAR    type %7s  lvl %2d  size %5d  offset %5d  storage_class %10s\n",
                        sym, sym->namestring, sym->datatype->namestring,
                        sym->blocklevel, sym->size, sym->offset, printStorageClass(sym));
            }
            else 
            {
                printf( " %6p  %10s  VAR    typ %7p  lvl %2d  siz %5d  off %5d\n",
                        sym, sym->namestring, sym->datatype,
                        sym->blocklevel, sym->size, sym->offset);
            }
            if (sym->datatype->kind != BASICTYPE )
            {
                ppsym(sym->datatype);
            }
            break;
        case TYPESYM:
            printf(" %6p  %10s  TYPE   typ %7p  lvl %2d  siz %5d  off %5d\n",
                    sym, sym->namestring, sym->datatype,
                    sym->blocklevel, sym->size, sym->offset);
            if (sym->datatype->kind != BASICTYPE )
            {
                ppsym(sym->datatype);
            }
            break;
        case BASICTYPE:
            printf(" %6p  %10s  BASIC  basicdt %3d          siz %5d\n",
                    sym, sym->namestring, sym->basicdt, sym->size);
            break;
        case SUBRANGE:
            printf(" %6p  %10s  SUBRA  typ %7d  val %5d .. %5d\n",
                    sym, sym->namestring, sym->basicdt,
                    sym->lowbound, sym->highbound);
            break;
        case CONSTSYM:
            switch (sym->basicdt)
            {  
                case INTEGER:
                    printf(" %6p  %10s  CONST  typ INTEGER  val  %12ld\n",
                            sym, sym->namestring, sym->constval.intnum);
                    break;
                //case REAL:
                //    printf(" %6p  %10s  CONST  typ    REAL  val  %12e\n",
                //            sym, sym->namestring, sym->constval.realnum);
                //    break;
                case STRING_TYPE:
                    printf(" %6p  %10s  CONST  typ  STRING  val  %12s\n",
                            sym, sym->namestring, sym->constval.stringconst);
                    break;
            }
            break;
    };
}

/* Print entries on one level of symbol table */
void printstlevel(int level)
{ 
    SYMBOL sym;
    sym =  symtab[level];
    if ( sym != NULL )
    { 
        printf("Symbol table level %d\n", level);
        while ( sym != NULL )
        { 
            printsymbol(sym);
            sym = sym->link;
        };
    };
}

/* Print all entries in the symbol table */
void printst(void)
{ 
    int level;
    for ( level = 0; level < MAXBLOCKS; level++) 
    {
        printstlevel(level);
    }
}

/* Insert a basic type into the symbol table */
SYMBOL insertbt(char name[], int basictp, int siz)
{ 
    SYMBOL sym;
    sym = insertsym(name);
    sym->kind = BASICTYPE;
    sym->basicdt = basictp;
    sym->size = siz;
    return sym;
}

/* Insert a one-argument function in the symbol table. */
/* Linked to the function symbol are result type followed by arg types.  */
SYMBOL insertfn(char name[], SYMBOL resulttp, SYMBOL argtp)
{ 
    SYMBOL sym, res, arg;
    sym = insertsym(name);
    sym->kind = FUNCTIONSYM;
    res = symalloc();
    res->kind = ARGSYM;
    res->datatype = resulttp;
    if (resulttp != NULL)
    {
        res->basicdt = resulttp->basicdt;
    }
    arg = symalloc();
    arg->kind = ARGSYM;
    arg->datatype = argtp;
    if (argtp != NULL)
    {
        arg->basicdt = argtp->basicdt;
    }
    arg->link = NULL;
    res->link = arg;
    sym->datatype = res;
    return sym;
}

/* Call this to initialize symbols provided by the compiler */
void initsyms(void)
{  
    SYMBOL sym; 
    //SYMBOL realsym; 
    //SYMBOL voidsym;
    SYMBOL intsym; 
    SYMBOL charsym; 
    SYMBOL shortsym; 
    SYMBOL longsym; 
    //SYMBOL boolsym;
    blocknumber = 0;               /* Put compiler symbols in block 0 */
    blockoffs[1] = BASEOFFSET;     /* offset of first variable */
    //realsym = insertbt("real", REAL, 8);
    //voidsym = insertbt("void", INTEGER, 0);
    charsym = insertbt("char", INTEGER, 1);
    shortsym  = insertbt("short", INTEGER, 2);
    intsym  = insertbt("int", INTEGER, 4);
    longsym  = insertbt("long", INTEGER, 8);


    //boolsym = insertbt("boolean", BOOLEAN_TYPE, 4);
    //sym = insertfn("exp", realsym, realsym);
    //sym = insertfn("sin", realsym, realsym);
    //sym = insertfn("cos", realsym, realsym);
    //sym = insertfn("trsin", realsym, realsym);
    //sym = insertfn("trcos", realsym, realsym);
    //sym = insertfn("sqrt", realsym, realsym);
    //sym = insertfn("round", intsym, realsym);
    //sym = insertfn("ord", intsym, intsym);
    //sym = insertfn("new", intsym, intsym);
    sym = insertfn("write", NULL, charsym);
    //sym = insertfn("writeln", NULL, charsym);
    //sym = insertfn("writef", NULL, realsym);
    //sym = insertfn("writelnf", NULL, realsym);
    //sym = insertfn("writei", NULL, intsym);
    //sym = insertfn("writelni", NULL, intsym);
    //sym = insertfn("read", NULL, NULL);
    //sym = insertfn("readln", NULL, NULL);
    //sym = insertfn("eof", boolsym, NULL);
    blocknumber = 1;             /* Start the user program in block 1 */
    contblock[1] = 0;
}

//copies the contents of the original to the destination
void copy_symbol(SYMBOL original, SYMBOL destination)
{
    memcpy(destination, original, sizeof(SYMBOLREC));
}

//sets storage class of a symbol

//FIXME: this is a hack to avoid circular dependencies between token.h and symtab.h
//figure out how to fix this properly
void setStorageClass(SYMBOL s, StorageClass whichStorage)
{
    s->storageclass = whichStorage;
}
