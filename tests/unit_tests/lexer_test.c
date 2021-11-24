#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"

void test_lexer(char *script, size_t size, struct token_info expected[])
{
    lexer_start(script, strlen(script));
    struct token_info actual_value;
    for (size_t i = 0; i < size; i++)
    {
        actual_value = pop_token();
        assert(actual_value.type == expected[i].type);

       if (actual_value.command != NULL && expected[i].command != NULL)
            assert(strcmp(actual_value.command, expected[i].command) == 0);
    }
    if (get_next_token().type != T_EOF)
    {
        assert(get_next_token().type == T_EOF);
    }
    
    lexer_reset();
}

void test_command1()
{
    char *script = "(ls -la)";
    struct token_info expected[] = { { T_O_PRTH, NULL },
                                     { T_WORD, "ls" },
                                     { T_WORD, "-la" },
                                     { T_C_PRTH, NULL } };

    printf("--  TEST SIMPLE COMMAND STARTED\n");

    test_lexer(script, 4, expected);

    printf("[TEST SIMPLE COMMAND GOOD]\n\n");
}

void test_command2()
{
    char *script = "echo test\n";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_NEWLINE, NULL } };

    printf("--  TEST SIMPLE COMMAND 2 STARTED\n\n");

    test_lexer(script, 3, expected);

    printf("[TEST SIMPLE COMMAND 2 GOOD]\n\n");
}

void test_command3()
{
    char *script = "ls -la";
    struct token_info expected[] = { { T_WORD, "ls" }, { T_WORD, "-la" } };

    printf("--  TEST SIMPLE COMMAND 3 STARTED\n\n");

    test_lexer(script, 2, expected);

    printf("[TEST SIMPLE COMMAND 3 GOOD]\n\n");
}

void test_command_list()
{
    char *script = "echo test; echo test";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_SEMICOLON, NULL },
                                     { T_WORD, "echo" },
                                     { T_WORD, "test" }};

    printf("--  TEST SIMPLE COMMAND LIST STARTED\n");

    test_lexer(script, 5, expected);

    printf("[TEST SIMPLE COMMAND LIST GOOD]\n\n");
}

void test_command_list2()
{
    char *script = "echo test; (echo test)";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_SEMICOLON, NULL },
                                     { T_O_PRTH, NULL },
                                     { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_C_PRTH, NULL } };

    printf("--  TEST SIMPLE COMMAND LIST2 STARTED\n");

    test_lexer(script, 7, expected);

    printf("[TEST SIMPLE COMMAND LIST2 GOOD]\n\n");
}

void test_command_list3()
{
    char *script = "echo test; ((echo test))";
    struct token_info expected[] = {
        { T_WORD, "echo" }, { T_WORD, "test" },
        { T_SEMICOLON, NULL },
        { T_O_PRTH, NULL },         { T_O_PRTH, NULL },
        { T_WORD, "echo" }, { T_WORD, "test" }, { T_C_PRTH, NULL },
        { T_C_PRTH, NULL }
    };

    printf("--  TEST SIMPLE COMMAND LIST3 STARTED\n");

    test_lexer(script, 9, expected);

    printf("[TEST SIMPLE COMMAND LIST3 GOOD]\n\n");
}

void test_command_if()
{
    char *script = "if true; then\n echo test\n fi";
    struct token_info expected[] = { { T_IF, NULL },
                                     { T_WORD, "true" },
                                     { T_SEMICOLON, NULL },
                                     { T_THEN, NULL },
                                     { T_NEWLINE, NULL },
                                     {T_WORD, "echo"},
                                     { T_WORD, "test" },
                                     {T_NEWLINE, NULL},
                                     { T_FI, NULL }};

    printf("--  TEST IF STARTED\n");

    test_lexer(script, 9, expected);

    printf("[TEST IF GOOD]\n\n");
}

void test_command_wrong_if()
{
    char *script = "ifecho true; then\n echo test\n fi";
    struct token_info expected[] = { { T_WORD, "ifecho" },
        { T_WORD, "true" },
        { T_SEMICOLON, NULL },
        { T_THEN, NULL },
        { T_NEWLINE, NULL },
        {T_WORD, "echo"},
        { T_WORD, "test" },
        {T_NEWLINE, NULL},
        { T_FI, NULL }};

    printf("--  TEST WRONG IF STARTED\n");

    test_lexer(script, 9, expected);

    printf("[TEST WRONG IF GOOD]\n\n");
}

void test_command_elif()
{
    char *script = "if true; then\n echo test\nelif false; then\n   echo true\nelse\n   'echo' '&&;;echo grosse merde'\nfi";
    struct token_info expected[] = { { T_IF, NULL },
        { T_WORD, "true" },
        { T_SEMICOLON, NULL },
        { T_THEN, NULL },
        { T_NEWLINE, NULL },
        {T_WORD, "echo"},
        {T_WORD, "test"},
        {T_NEWLINE, NULL},
        {T_ELIF, NULL},
        { T_WORD, "false" },
        { T_SEMICOLON, NULL },
        { T_THEN, NULL },
        { T_NEWLINE, NULL },
        {T_WORD, "echo"},
        {T_WORD, "true"},
        { T_NEWLINE, NULL },
        { T_ELSE, NULL },
        { T_NEWLINE, NULL },
        {T_WORD, "echo"},
        {T_WORD, "&&;;echo grosse merde"},
        { T_NEWLINE, NULL },
        { T_FI, NULL }};

    printf("--  TEST IF STARTED\n");

    test_lexer(script, 22, expected);

    printf("[TEST IF GOOD]\n\n");
}

void test_next()
{
    char *script = "if true; then\n echo test\n fi";
    lexer_start(script, strlen(script));
    printf("--  TEST NEXT STARTED\n");

    for (int i = 0; i < 800; i++)
    {
        assert(get_next_token().type == T_IF);
    }

    printf("[TEST NEXT GOOD]\n\n");
    lexer_reset();
}

/*void test_complex_quote()
{
    char *script = " i'f''&&'&&'ec'ho '%% pute'";
    struct token_info expected[] = { { T_WORD, "ifecho" },
        { T_WORD, "true" },
        { T_SEMICOLON, NULL },
        { T_THEN, NULL },
        { T_NEWLINE, NULL },
        {T_WORD, "echo"},
        { T_WORD, "test" },
        {T_NEWLINE, NULL},
        { T_FI, NULL }};

    printf("--  TEST WRONG IF STARTED\n");

    test_lexer(script, 9, expected);

    printf("[TEST WRONG IF GOOD]\n\n");
}*/

int main(void)
{
    test_command1();
    test_command2();
    test_command3();
    test_command_list();
    test_command_list2();
    test_command_list3();
    test_command_if();
    test_next();
    test_command_wrong_if();
    test_command_elif();

    char *script = "echo ''''";
    lexer_start(script, strlen(script));
    struct token_info tk;
    while ((tk = pop_token()).type != T_EOF)
        continue;

    printf("[ALL GOOD !]\n");
}
