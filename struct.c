/*
 * =====================================================================================
 *
 *       Filename:  struct.c
 *
 *    Description:  This file contains helper routines for parsing structs (and unions)
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

#include "token.h"
#define NULL 0 //FIXME this shouldn't be needed. find out why


// <struct-or-union-specifier> ::= <struct-or-union> <identifier>? {  <struct-declaration-list>  }  |
//                                  <struct-or-union> <identifier>

TOKEN struct_or_union_specifier(void)
{
    TOKEN t = NULL;

    return t;
}

// <struct-or-union> ::= STRUCT |
//                      UNION

TOKEN struct_or_union(void)
{
    TOKEN t = NULL;

    return t;

}

// <struct-declaration-list> ::= <struct-declaration> <struct-declaration-list>*

TOKEN struct_declaration_list(void)
{
    TOKEN t = NULL;

    return t;

}

// <struct-declaration> ::= <specifier-qualifier-list> <struct-declarator-list> ;
//

TOKEN struct_declaration(void)
{
    TOKEN t = NULL;

    return t;

}

// <specifier-qualifier-list> ::= <type-specifier> <specifier-qualifier-list>? |
//                                <type-qualifier> <specifier-qualifier-list>?
TOKEN specifier_qualifier_list(void)
{
    TOKEN t = NULL;

    return t;
}

// <struct-declarator-list> ::= <struct-declarator> |
//                              <struct-declarator> , <struct-declarator-list>

TOKEN struct_declarator_list(void)
{
    TOKEN t = NULL;

    return t;
}

// <struct-declarator> ::= <declarator> |
//                         <declarator>? : <constant-expression>

TOKEN struct_declarator(void)
{
    TOKEN t = NULL;

    return t;
}
