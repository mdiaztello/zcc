#include "CppUTest/TestHarness.h"

extern "C"
{
#include <stdio.h>
#include "scanner.h"
#include "lexer.h"
#include "token.h"
#include "token_API.h"
}

TEST_GROUP(LEXER_TESTS)
{
    void setup(void)
    {
    }

    void teardown(void)
    {
    }

};

TEST(LEXER_TESTS, FakeTest)
{
    FILE* input_file = fopen("tests/testc", "r+");
    init_scanner(input_file);
    TOKEN tok = gettok();
    if( getTokenType(tok) != IDENTIFIER_TOKEN)
    {
        FAIL("BOO!");
    }
}



