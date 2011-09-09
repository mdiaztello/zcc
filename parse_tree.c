
#include "token.h"
#include "token_API.h"
#include <stdio.h>

//contains helper functions for constructing the parse tree for the program


TOKEN make_binary_operation(TOKEN operation, TOKEN leftSide, TOKEN rightSide)
{
    setOperands(operation, leftSide);
    setLink(leftSide, rightSide);
    setLink(rightSide, NULL);
    return operation;
}


TOKEN make_unary_operation(TOKEN operation, TOKEN operand)
{
    setOperands(operation, operand);
    setLink(operand, NULL);
    return operation;
}
