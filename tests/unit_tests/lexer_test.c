#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"

void test_lexer(char *script, size_t size, struct token_info expected[])
{
    struct token_info actual_value;
    size_t len = strlen(script);
    for (size_t i = 0; i < size; i++)
    {
        actual_value = pop_token(script, len);
        assert(actual_value.type == expected[i].type);

        if (actual_value.command != NULL && expected[i].command != NULL)
            assert(strcmp(actual_value.command, expected[i].command) == 0);
    }
    if (get_next_token(script, len).type != T_EOF)
    {
        assert(get_next_token(script, len).type == T_EOF);
    }
}

void test_command1()
{
    char *script = "(ls -la)";
    struct token_info expected[] = { { T_O_PRTH, NULL },
                                     { T_COMMAND, "ls -la" },
                                     { T_C_PRTH, NULL } };

    printf("--  TEST SIMPLE COMMAND STARTED\n");

    test_lexer(script, 3, expected);

    printf("[TEST SIMPLE COMMAND GOOD]\n\n");
}

void test_command2()
{
    char *script = "echo test\n";
    struct token_info expected[] = { { T_COMMAND, "echo test" },
                                     { T_NEWLINE, NULL } };

    printf("--  TEST SIMPLE COMMAND 2 STARTED\n\n");

    test_lexer(script, 2, expected);

    printf("[TEST SIMPLE COMMAND 2 GOOD]\n\n");
}

void test_command3()
{
    char *script = "ls -la";
    struct token_info expected[] = { { T_COMMAND, "ls -la" } };

    printf("--  TEST SIMPLE COMMAND 3 STARTED\n\n");

    test_lexer(script, 1, expected);

    printf("[TEST SIMPLE COMMAND 3 GOOD]\n\n");
}

void test_command_list()
{
    char *script = "echo test; echo test";
    struct token_info expected[] = { { T_COMMAND, "echo test" },
                                     { T_SEMICOLON, NULL },
                                     { T_COMMAND, "echo test" } };

    printf("--  TEST SIMPLE COMMAND LIST STARTED\n");

    test_lexer(script, 3, expected);

    printf("[TEST SIMPLE COMMAND LIST GOOD]\n\n");
}

void test_command_list2()
{
    char *script = "echo test; (echo test)";
    struct token_info expected[] = { { T_COMMAND, "echo test" },
                                     { T_SEMICOLON, NULL },
                                     { T_O_PRTH, NULL },
                                     { T_COMMAND, "echo test" },
                                     { T_C_PRTH, NULL } };

    printf("--  TEST SIMPLE COMMAND LIST2 STARTED\n");

    test_lexer(script, 5, expected);

    printf("[TEST SIMPLE COMMAND LIST2 GOOD]\n\n");
}

void test_command_list3()
{
    char *script = "echo test; ((echo test))";
    struct token_info expected[] = {
        { T_COMMAND, "echo test" }, { T_SEMICOLON, NULL },
        { T_O_PRTH, NULL },         { T_O_PRTH, NULL },
        { T_COMMAND, "echo test" }, { T_C_PRTH, NULL },
        { T_C_PRTH, NULL }
    };

    printf("--  TEST SIMPLE COMMAND LIST3 STARTED\n");

    test_lexer(script, 7, expected);

    printf("[TEST SIMPLE COMMAND LIST3 GOOD]\n\n");
}

void test_command_if()
{
    char *script = "if true; then\n echo test\n fi";
    struct token_info expected[] = { { T_IF, NULL },
                                     { T_COMMAND, "true" },
                                     { T_SEMICOLON, NULL },
                                     { T_THEN, NULL },
                                     { T_NEWLINE, NULL },
                                     {T_COMMAND, "echo test"},
                                     {T_NEWLINE, NULL},
                                     { T_FI, NULL }};

    printf("--  TEST IF STARTED\n");

    test_lexer(script, 8, expected);

    printf("[TEST IF GOOD]\n\n");
}

void test_next()
{
    char *script = "if true; then\n echo test\n fi";
    printf("--  TEST NEXT STARTED\n");

    for (int i = 0; i < 800; i++)
    {
        assert(get_next_token(script, strlen(script)).type == T_IF);
    }

    printf("[TEST NEXT GOOD]\n\n");
}

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

    printf("[ALL GOOD !]\n");
}
