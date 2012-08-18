#ifndef INCLUDE_GUARD__TOKEN_API_H_
#define INCLUDE_GUARD__TOKEN_API_H_

/*
 * =====================================================================================
 *
 *       Filename:  token_API.h
 *
 *    Description:  Header file for all of the operations that can be done on token 
 *                  structures. The function prototypes are in this separate header
 *                  in order to prevent circular dependencies from forming when other
 *                  headers need access to the token struct declaration but not it's 
 *                  function prototypes.
 *
 *        Version:  1.0
 *        Created:  8/31/2011 13:52:50
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Michael Diaz-Tello (MDT) mdiaz-tello@gmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdbool.h>
#include "symtab.h"

TOKEN makeToken(void);
void setTokenType(TOKEN t, enum token_type tType);
TokenType getTokenType(TOKEN t);
void setDataType(TOKEN t, DataType dType);
DataType getDataType(TOKEN t);

void setStringVal(TOKEN t, char* string);
char* getStringVal(TOKEN t);

unsigned int getWhichVal(TOKEN t);
void setWhichVal(TOKEN t, int which);

void setIntegerValue(TOKEN t, long intvalue);
long getIntegerValue(TOKEN t);

TOKEN getLink(TOKEN tok);
void setLink(TOKEN tok, TOKEN linked_tok);

void setOperands(TOKEN tok, TOKEN operand);
TOKEN getOperands(TOKEN tok);

SYMBOL getSymbolType(TOKEN tok);
void setSymbolType(TOKEN tok, SYMBOL sym_t);

SYMBOL getSymbolTableEntry(TOKEN tok);
void setSymbolTableEntry(TOKEN tok, SYMBOL sym_entry);

double getRealVal(TOKEN tok);
void setRealVal(TOKEN tok, double value);

StorageClass getTokenStorageClass(TOKEN tok);

bool isKeyword(TOKEN tok);
bool isOperator(TOKEN tok);
bool isAssignmentOperator(TOKEN tok);
bool isUnaryOperator(TOKEN tok);
bool isMultiplicativeOperator(TOKEN tok);
bool isAdditiveOperator(TOKEN tok);
bool isDelimiter(TOKEN tok);

bool isIterationKeyword(TOKEN tok);
bool isJumpKeyword(TOKEN tok);
bool isSelectionKeyword(TOKEN tok);


bool reserved(TOKEN tok, KeywordType keyword);
bool _operator(TOKEN tok, OperatorType _operator);
bool delimiter(TOKEN tok, DelimiterType delim);

void printToken(TOKEN t);
void printTokenType(TOKEN t);
void printKeywordType(TOKEN t);
void printIdentifier(TOKEN t);
void printDelimiterType(TOKEN t);
void printNumericValue(TOKEN t);
void printOperator(TOKEN t);

#endif //INCLUDE_GUARD__TOKEN_API_H_
