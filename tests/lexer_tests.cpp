#include "CppUTest/TestHarness.h"

extern "C"
{
#include <stdio.h>
#include "scanner.h"
#include "lexer.h"
#include "token.h"
#include "token_API.h"
#include "token.h"
}

TEST_GROUP(LEXER_TESTS)
{
    FILE* input_file;

    void setup(void)
    {
    }

    void teardown(void)
    {
        fclose(input_file);
    }

    void set_input_source(char* file_name, char* mode)
    {
        char file_path[100];
        char* test_directory = "tests/lexer_tests/";
        sprintf(file_path, "%s%s",test_directory, file_name);
        input_file = fopen(file_path, mode);
        if(NULL == input_file)
        {
            printf("couldn't open file %s...\n", file_path);
            exit(EXIT_FAILURE); //bail out
        }
        init_scanner(input_file);
    }
};

//make sure that when we peek, it grabs the next input
//without advancing the input pointer
TEST(LEXER_TESTS, ConsecutivePeekToksDoNotAdvanceInput)
{
    set_input_source("peektok_test", "r+");
    TOKEN tok1 = peek_token();
    for(int i = 0; i < 10; i++)
    {
        peek_token();
    }
    TOKEN tok2 = peek_token();
    CHECK(get_token_type(tok1) == IDENTIFIER_TOKEN);
    CHECK(get_token_type(tok2) == IDENTIFIER_TOKEN);
    STRCMP_EQUAL(getStringVal(tok1), getStringVal(tok2));
    POINTERS_EQUAL(tok1, tok2);
}

TEST(LEXER_TESTS, GetTokAdvancesInputOnConsecutiveCalls)
{
    set_input_source("gettok_test", "r+");
    TOKEN tok1 = get_token();
    TOKEN tok2 = get_token();
    STRCMP_EQUAL(getStringVal(tok1), "a");
    STRCMP_EQUAL(getStringVal(tok2), "b");
    CHECK(tok1 != tok2);
}

TEST(LEXER_TESTS, GetTokReturnsPeekedTokAfterPeekTokCall)
{
    set_input_source("gettok_test", "r+");
    TOKEN get1 = get_token();
    TOKEN peek = peek_token();
    TOKEN get2 = get_token();
    STRCMP_EQUAL("a", getStringVal(get1));
    STRCMP_EQUAL("b", getStringVal(peek));
    STRCMP_EQUAL("b", getStringVal(get2));
    POINTERS_EQUAL(peek, get2);
}

TEST(LEXER_TESTS, LexerRecognizesKeywords)
{
    TOKEN tok;
    set_input_source("keyword_test", "r+");
    for(int keyword_type = STATIC; keyword_type < NUM_KEYWORD_TYPES; ++keyword_type)
    {
        tok = get_token();
        CHECK(true == reserved(tok, (KeywordType)keyword_type));
    }
}

TEST(LEXER_TESTS, LexerCanProcessIdentifiers)
{
    TOKEN tok;
    set_input_source("identifier_test", "r+");

    char* identifiers[4] = {
        "an_identifier_with_underscores", 
        "superduperunreasonablylongidentifiernamethatshouldnotcausethelexertochoke", 
        "_an_identifier_with_leading_underscore", 
        "an_identifier_with_numbers_123_and_underscores12" };

    for(int i = 0; i < 4; i++)
    {
        tok = get_token();
        STRCMP_EQUAL(identifiers[i], getStringVal(tok));
    }
}

TEST(LEXER_TESTS, LexerHandlesWeirdInlineComments)
{
    TOKEN tok;
    set_input_source("tricky_comments_test", "r+");
    char* expected_identifiers[6] = { "u", "v", "w", "x", "y", "z" };

    for(int i = 0; i < 6; i++)
    {
        tok = get_token();
        CHECK(false == isKeyword(tok));
        STRCMP_EQUAL(expected_identifiers[i], getStringVal(tok));
    }
}

TEST(LEXER_TESTS, LexerDetectsDelimiters)
{
    TOKEN tok;
    set_input_source("delimiter_test", "r+");

    for(int delimiter_type = COMMA; delimiter_type < NUM_DELIMITER_TYPES; delimiter_type++)
    {
        tok = get_token();
        CHECK(true == delimiter(tok, (DelimiterType)delimiter_type));
    }
}

TEST(LEXER_TESTS, LexterProperlyDetectsOperators)
{
    TOKEN tok;

    set_input_source("operator_test", "r+");

    for(int op_type = ADDITION; op_type < STAR; op_type++)
    {
        tok = get_token();
        if((op_type == MULTIPLICATION) || (op_type == DEREFERENCE))//the lexer reports these as "STAR" tokens until the parser can disambiguate them
        {
            CHECK(true == _operator(tok, STAR));
        }
        else if((op_type == BITWISE_AND) || (op_type == REFERENCE))//the lexer reports these as "AMPERSAND" tokens until the parser can disambiguate them
        {
            CHECK(true == _operator(tok, AMPERSAND));
        }
        else
        {
            CHECK(true == _operator(tok, (OperatorType)op_type));
        }
    }
}

TEST(LEXER_TESTS, LexerUsesMaximalMunchRule)
{
    OperatorType expectedOperators[] = { 
        INCREMENT, INCREMENT, ADDITION, INCREMENT, 
        ASSIGNMENT, PLUS_EQUAL, ASSIGNMENT, BOOLEAN_AND, 
        ASSIGNMENT, BOOLEAN_AND, EQUALS, BOOLEAN_OR, 
        ASSIGNMENT, BOOLEAN_OR, BITWISE_OR_EQUAL, BITWISE_OR,
        INCREMENT, INCREMENT, INCREMENT, PLUS_EQUAL, 
        PLUS_EQUAL, ASSIGNMENT, BOOLEAN_AND, ASSIGNMENT, 
        BOOLEAN_AND, EQUALS, BOOLEAN_OR, ASSIGNMENT, 
        BOOLEAN_OR, BITWISE_OR_EQUAL, BITWISE_OR
    };

    int i = 0;
    set_input_source("tricky_operator_test", "r+");
    TOKEN tok = get_token();
    
    while(tok != NULL)
    {
        CHECK(true == _operator(tok, expectedOperators[i]));
        tok = get_token();
        i++;
    }
}

