#include <criterion/criterion.h>
#include <string.h>

#include "lexer.h"

void test_lexer(char *script, size_t size, struct token_info expected[])
{
    lexer_start(script, strlen(script));
    struct token_info actual_value;
    for (size_t i = 0; i < size; i++)
    {
        actual_value = pop_token();
        cr_assert_eq(actual_value.type, expected[i].type,
                     "Expected : %d | Got : %d", actual_value.type,
                     expected[i].type);

        if (actual_value.command != NULL && expected[i].command != NULL)
            cr_assert_str_eq(actual_value.command, expected[i].command,
                             "Expected : %s | Got : %s", actual_value.command,
                             expected[i].command);
    }
    if (get_next_token().type != T_EOF)
    {
        cr_assert_eq(actual_value.type, T_EOF, "Expected : %d | Got : %d",
                     actual_value.type, T_EOF);
    }

    lexer_reset();
}

Test(PARENTHESIS, SIMPLE_LS)
{
    char *script = "(ls -la)";
    struct token_info expected[] = { { T_O_PRTH, NULL },
                                     { T_WORD, "ls" },
                                     { T_WORD, "-la" },
                                     { T_C_PRTH, NULL } };

    test_lexer(script, 4, expected);
}

Test(COMMAND, SIMPLE)
{
    char *script = "echo test\n";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_NEWLINE, NULL } };

    test_lexer(script, 3, expected);
}

Test(COMMAND, SIMPLE_LS)
{
    char *script = "ls -la";
    struct token_info expected[] = { { T_WORD, "ls" }, { T_WORD, "-la" } };

    test_lexer(script, 2, expected);
}

Test(COMMAND, LIST)
{
    char *script = "echo test; echo test";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_SEMICOLON, NULL },
                                     { T_WORD, "echo" },
                                     { T_WORD, "test" } };

    test_lexer(script, 5, expected);
}

Test(PARENTHIS, COMMAND_LIST)
{
    char *script = "echo test; (echo test)";
    struct token_info expected[] = { { T_WORD, "echo" },    { T_WORD, "test" },
                                     { T_SEMICOLON, NULL }, { T_O_PRTH, NULL },
                                     { T_WORD, "echo" },    { T_WORD, "test" },
                                     { T_C_PRTH, NULL } };

    test_lexer(script, 7, expected);
}

Test(PARENTHESIS, DOUBLE)
{
    char *script = "echo test; ((echo test))";
    struct token_info expected[] = { { T_WORD, "echo" },    { T_WORD, "test" },
                                     { T_SEMICOLON, NULL }, { T_O_PRTH, NULL },
                                     { T_O_PRTH, NULL },    { T_WORD, "echo" },
                                     { T_WORD, "test" },    { T_C_PRTH, NULL },
                                     { T_C_PRTH, NULL } };

    test_lexer(script, 9, expected);
}

Test(IF, SIMPLE)
{
    char *script = "if true; then\n echo test\n fi";
    struct token_info expected[] = { { T_IF, NULL },        { T_WORD, "true" },
                                     { T_SEMICOLON, NULL }, { T_THEN, NULL },
                                     { T_NEWLINE, NULL },   { T_WORD, "echo" },
                                     { T_WORD, "test" },    { T_NEWLINE, NULL },
                                     { T_FI, NULL } };

    test_lexer(script, 9, expected);
}

Test(IF, WRONG)
{
    char *script = "ifecho true; then\n echo test\n fi";
    struct token_info expected[] = { { T_WORD, "ifecho" },  { T_WORD, "true" },
                                     { T_SEMICOLON, NULL }, { T_THEN, NULL },
                                     { T_NEWLINE, NULL },   { T_WORD, "echo" },
                                     { T_WORD, "test" },    { T_NEWLINE, NULL },
                                     { T_FI, NULL } };

    test_lexer(script, 9, expected);
}

Test(IF, HARD_QUOTE)
{
    char *script = "if true; then\n echo test\nelif false; then\n   echo "
                   "true\nelse\n   'echo' '&&;;echo grosse merde'\nfi";
    struct token_info expected[] = {
        { T_IF, NULL },        { T_WORD, "true" },
        { T_SEMICOLON, NULL }, { T_THEN, NULL },
        { T_NEWLINE, NULL },   { T_WORD, "echo" },
        { T_WORD, "test" },    { T_NEWLINE, NULL },
        { T_ELIF, NULL },      { T_WORD, "false" },
        { T_SEMICOLON, NULL }, { T_THEN, NULL },
        { T_NEWLINE, NULL },   { T_WORD, "echo" },
        { T_WORD, "true" },    { T_NEWLINE, NULL },
        { T_ELSE, NULL },      { T_NEWLINE, NULL },
        { T_WORD, "echo" },    { T_WORD, "&&;;echo grosse merde" },
        { T_NEWLINE, NULL },   { T_FI, NULL }
    };

    test_lexer(script, 22, expected);
}

Test(QUOTE, EASY)
{
    char *script = "echo '&&'";
    struct token_info expected[] = { { T_WORD, "echo" },
                                    { T_WORD, "&&" } };

    test_lexer(script, 2, expected);
}

Test(QUOTE, MEDIUM)
{
    char *script = "echo      '      pute''";
    struct token_info expected[] = { { T_WORD, "echo" },
        { T_WORD, "      pute" } };

    test_lexer(script, 2, expected);
}

Test(QUOTE, HARD)
{
    char *script = " i'f''&&'&&'ec'ho '%% pute'";
    struct token_info expected[] = { { T_WORD, "if&&" },
                                     { T_AND, NULL },
                                     { T_WORD, "echo" },
                                     { T_WORD, "%% pute" } };

    test_lexer(script, 4, expected);
}

Test(REDIR, EASY)
{
    char *script = "echo 'test'>test.txt";
    struct token_info expected[] = { { T_WORD, "echo" },
        { T_WORD, "test" },
        { T_REDIR_1, NULL },
        { T_WORD, "test.txt" } };

    test_lexer(script, 4, expected);
}

/*int main(void)
{
    char *script = "echo ''''";
    lexer_start(script, strlen(script));
    struct token_info tk;
    while ((tk = pop_token()).type != T_EOF)
        continue;
}*/
