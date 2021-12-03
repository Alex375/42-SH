#include <criterion/criterion.h>
#include <string.h>

#include "lexer.h"

#define EXPECTED_SIZE(array) (sizeof((array)) / sizeof(struct token_info))

void test_lexer(char *script, size_t size, struct token_info expected[])
{
    lexer_start(script, strlen(script));
    struct token_info actual_value;
    for (size_t i = 0; i < size; i++)
    {
        actual_value = pop_token();
        cr_assert_eq(actual_value.type, expected[i].type,
                     "Got : %d | Expected : %d", actual_value.type,
                     expected[i].type);

        cr_assert_eq(actual_value.is_space_after, expected[i].is_space_after, "Expected : %d | Got : %d",
                     actual_value.is_space_after, expected[i].is_space_after);

        if (actual_value.command != NULL && expected[i].command != NULL)
            cr_assert_str_eq(actual_value.command, expected[i].command,
                             "Got : %s | Expected : %s", actual_value.command,
                             expected[i].command);
    }
    if (get_next_token().type != T_EOF)
    {
        cr_assert_eq(actual_value.type, T_EOF, "Expected : %d | Got : %d",
                     actual_value.type, T_EOF);
    }

    lexer_reset();
}

TestSuite(PARENTHESIS, .timeout = 1);
TestSuite(COMMAND, .timeout = 1);
TestSuite(IF, .timeout = 1);
TestSuite(WHILE, .timeout = 1);
TestSuite(FOR, .timeout = 1);
TestSuite(REDIR, .timeout = 1);
TestSuite(QUOTE, .timeout = 1);
TestSuite(AND, .timeout = 1);
TestSuite(OR, .timeout = 1);
TestSuite(ESCAPE, .timeout = 1);
TestSuite(DOUBLE_QUOTE, .timeout = 1);

Test(PARENTHESIS, SIMPLE_LS)
{
    char *script = "(ls -la)";
    struct token_info expected[] = { { T_O_PRTH, NULL, 0 },
                                     { T_WORD, "ls", 1 },
                                     { T_WORD, "-la", 0 },
                                     { T_C_PRTH, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(COMMAND, SIMPLE)
{
    char *script = "echo test\n";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0, },
                                     { T_NEWLINE, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(COMMAND, MEDIUM)
{
    char *script = "echo if \n";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "if", 1 },
                                     { T_NEWLINE, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(COMMAND, SIMPLE_LS)
{
    char *script = "ls -la";
    struct token_info expected[] = { { T_WORD, "ls", 1 }, { T_WORD, "-la", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(COMMAND, LIST)
{
    char *script = "echo test; echo test";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0 },
                                     { T_SEMICOLON, NULL, 1 },
                                     { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(PARENTHIS, COMMAND_LIST)
{
    char *script = "echo test; (echo test)";
    struct token_info expected[] = { { T_WORD, "echo", 1 },    { T_WORD, "test", 0 },
                                     { T_SEMICOLON, NULL, 1 }, { T_O_PRTH, NULL, 0 },
                                     { T_WORD, "echo", 1 },    { T_WORD, "test", 0 },
                                     { T_C_PRTH, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(PARENTHESIS, DOUBLE)
{
    char *script = "echo test; ((echo test))";
    struct token_info expected[] = { { T_WORD, "echo", 1 },    { T_WORD, "test", 0 },
                                     { T_SEMICOLON, NULL, 1 }, { T_O_PRTH, NULL,0 },
                                     { T_O_PRTH, NULL,0 },    { T_WORD, "echo",1 },
                                     { T_WORD, "test",0 },    { T_C_PRTH, NULL,0 },
                                     { T_C_PRTH, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(IF, SIMPLE)
{
    char *script = "if true; then\n echo test\n fi";
    struct token_info expected[] = { { T_IF, NULL, 1 },        { T_WORD, "true", 0 },
                                     { T_SEMICOLON, NULL, 1 }, { T_THEN, NULL, 0 },
                                     { T_NEWLINE, NULL, 1 },   { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0 },    { T_NEWLINE, NULL, 1 },
                                     { T_FI, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(IF, WRONG)
{
    char *script = "ifecho true; then\n echo test\n fi";
    struct token_info expected[] = { { T_WORD, "ifecho", 1 },  { T_WORD, "true", 0 },
                                     { T_SEMICOLON, NULL, 1 }, { T_THEN, NULL, 0 },
                                     { T_NEWLINE, NULL, 1 },   { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0 },    { T_NEWLINE, NULL, 1 },
                                     { T_FI, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(IF, HARD_QUOTE)
{
    char *script = "if true; then\n echo test\nelif false; then\n   echo "
                   "true\nelse\n   'echo' '&&;;echo gentil monsieur'\nfi";
    struct token_info expected[] = {
        { T_IF, NULL, 1 },        { T_WORD, "true", 0 },
        { T_SEMICOLON, NULL, 1 }, { T_THEN, NULL, 0 },
        { T_NEWLINE, NULL, 1 },   { T_WORD, "echo", 1 },
        { T_WORD, "test", 0 },    { T_NEWLINE, NULL, 0 },
        { T_ELIF, NULL, 1 },      { T_WORD, "false", 0},
        { T_SEMICOLON, NULL, 1 }, { T_THEN, NULL, 0 },
        { T_NEWLINE, NULL, 1 },   { T_WORD, "echo", 1 },
        { T_WORD, "true", 0 },    { T_NEWLINE, NULL, 0 },
        { T_ELSE, NULL, 0 },      { T_NEWLINE, NULL, 1 },
        { T_WORD, "echo", 1 },    { T_WORD, "&&;;echo gentil monsieur", 0 },
        { T_NEWLINE, NULL, 0 },   { T_FI, NULL, 0 }
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, EASY)
{
    char *script = "echo '&&'";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "&&", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, MEDIUM)
{
    char *script = "echo      '      salut'";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "      salut", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, EMPTY)
{
    char *script = "echo  ''''";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, HARD)
{
    char *script = " i'f''&&'&&'ec'ho '%% salut'";
    struct token_info expected[] = { { T_WORD, "if&&", 0 },
                                     { T_AND, NULL, 0 },
                                     { T_WORD, "echo", 1 },
                                     { T_WORD, "%% salut", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, ERROR)
{
    char *script = "echo ''cest une erreur'";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "cest", 1 },
                                     { T_WORD, "une", 1 },
                                     { T_ERROR, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, IF)
{
    char *script = "i'f'";
    struct token_info expected[] = { { T_WORD, "if", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, EASY)
{
    char *script = "echo test>test.txt";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0 },
                                     { T_REDIR_1, NULL, 0},
                                     { T_WORD, "test.txt", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, EASY2)
{
    char *script = "echo test<test.txt";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0 },
                                     { T_REDIR_I_1, NULL, 0 },
                                     { T_WORD, "test.txt", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM)
{
    char *script = "echo test>&test.txt";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0 },
                                     { T_REDIR_A, NULL, 0 },
                                     { T_WORD, "test.txt", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM2)
{
    char *script = "echo test<&test.txt";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0 },
                                     { T_REDIR_I_A, NULL, 0 },
                                     { T_WORD, "test.txt", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM3)
{
    char *script = "echo test>>test.txt";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0 },
                                     { T_REDIR_2, NULL, 0 },
                                     { T_WORD, "test.txt", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM4)
{
    char *script = "echo test<>test.txt";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0 },
                                     { T_REDIR_O_2, NULL, 0 },
                                     { T_WORD, "test.txt", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM5)
{
    char *script = "echo test>|test.txt";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0 },
                                     { T_REDIR_PIPE, NULL, 0 },
                                     { T_WORD, "test.txt", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, LONG)
{
    char *script = "echo test 1023>test.txt ";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 1 },
                                     { T_WORD, "1023", 0 },
                                     { T_REDIR_1, NULL, 0 },
                                     { T_WORD, "test.txt", 1 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM_io)
{
    char *script = "echo test 2>|test.txt";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 1 },
                                     { T_IONUMBER, "2", 0 },
                                     { T_REDIR_PIPE, NULL, 0 },
                                     { T_WORD, "test.txt", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM2_io)
{
    char *script = "echo test 2<test.txt";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 1 },
                                     { T_IONUMBER, "2", 0 },
                                     { T_REDIR_I_1, NULL, 0 },
                                     { T_WORD, "test.txt", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM2_WRONG_IO)
{
    char *script = "echo test hello<test.txt";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 1 },
                                     { T_WORD, "hello", 0 },
                                     { T_REDIR_I_1, NULL, 0 },
                                     { T_WORD, "test.txt", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, HARD)
{
    char *script = "echo test88> test.txt";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test88", 0 },
                                     { T_REDIR_1, NULL, 1 },
                                     { T_WORD, "test.txt", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(OR, EASY)
{
    char *script = "echo test || echo test";
    struct token_info expected[] = {
        { T_WORD, "echo", 1 }, { T_WORD, "test", 1 }, { T_OR, NULL, 1 },
        { T_WORD, "echo", 1 }, { T_WORD, "test", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(OR, MEDIUM)
{
    char *script = "echo test||echo test";
    struct token_info expected[] = {
        { T_WORD, "echo", 1 }, { T_WORD, "test", 0 }, { T_OR, NULL, 0 },
        { T_WORD, "echo", 1 }, { T_WORD, "test", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(AND, MEDIUM)
{
    char *script = "echo test&&echo test";
    struct token_info expected[] = {
        { T_WORD, "echo", 1 }, { T_WORD, "test", 0 }, { T_AND, NULL, 0 },
        { T_WORD, "echo", 1 }, { T_WORD, "test", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(WHILE, EASY)
{
    char *script = "while true\ndo\necho hello\ndone";
    struct token_info expected[] = { { T_WHILE, NULL, 1 },   { T_WORD, "true", 0 },
                                     { T_NEWLINE, NULL, 0 }, { T_DO, NULL, 0 },
                                     { T_NEWLINE, NULL, 0 }, { T_WORD, "echo", 1 },
                                     { T_WORD, "hello", 0 }, { T_NEWLINE, NULL, 0 },
                                     { T_DONE, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(UNTIL, EASY)
{
    char *script = "until true\ndo\necho hello\ndone";
    struct token_info expected[] = { { T_UNTIL, NULL, 1 },   { T_WORD, "true", 0 },
                                     { T_NEWLINE, NULL, 0 }, { T_DO, NULL, 0 },
                                     { T_NEWLINE, NULL, 0 }, { T_WORD, "echo", 1 },
                                     { T_WORD, "hello", 0 }, { T_NEWLINE, NULL, 0 },
                                     { T_DONE, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(COMMAND, SPACES_AFTER)
{
    char *script = "echo coucou ";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "coucou", 1 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(EMPTY, SIMPLE)
{
    char *script = "";
    struct token_info *expected = NULL;

    test_lexer(script, 0, expected);
}

Test(EMPTY, MEIDUM)
{
    char *script = "    ";
    struct token_info *expected = NULL;

    test_lexer(script, 0, expected);
}

Test(EMPTY, QUOTE)
{
    char *script = "''";
    struct token_info expected[] = { { T_WORD, "", 0 } };

    test_lexer(script, 1, expected);
}

Test(FOR, EASY)
{
    char *script = "for test in 1 2 3\ndo\n echo 'jadore h'\ndone";
    struct token_info expected[] = {
        { T_FOR, NULL, 1 },     { T_VAR, "test", 1 },      { T_IN, NULL, 1 },
        { T_WORD, "1", 1 },     { T_WORD, "2", 1 },        { T_WORD, "3", 0 },
        { T_NEWLINE, NULL, 0}, { T_DO, NULL, 0  },         { T_NEWLINE, NULL, 1 },
        { T_WORD, "echo", 1 },  { T_WORD, "jadore h", 0 }, { T_NEWLINE, NULL, 0 },
        { T_DONE, NULL, 0 }
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, MEDIUM)
{
    char *script = "for in in in; do\n  test\ndone";
    struct token_info expected[] = { { T_FOR, NULL, 1 },       { T_VAR, "in", 1 },
                                     { T_IN, NULL, 1 },        { T_WORD, "in", 0 },
                                     { T_SEMICOLON, NULL, 1 }, { T_DO, NULL, 0 },
                                     { T_NEWLINE, NULL, 1 },   { T_WORD, "test", 0 },
                                     { T_NEWLINE, NULL, 0 },   { T_DONE, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, A_LOT)
{
    char *script = "for in in in in in in in in in in in in in in in in in in "
                   "in in in in in in in; do\n  echo bonjour!\ndone";
    struct token_info expected[] = {
        { T_FOR, NULL, 1 },     { T_VAR, "in", 1 },       { T_IN, NULL, 1 },
        { T_WORD, "in", 1 },    { T_WORD, "in", 1 },      { T_WORD, "in", 1 },
        { T_WORD, "in", 1 },    { T_WORD, "in", 1 },      { T_WORD, "in", 1 },
        { T_WORD, "in", 1 },    { T_WORD, "in", 1 },      { T_WORD, "in", 1 },
        { T_WORD, "in", 1 },    { T_WORD, "in", 1 },      { T_WORD, "in", 1 },
        { T_WORD, "in", 1 },    { T_WORD, "in", 1 },      { T_WORD, "in", 1 },
        { T_WORD, "in", 1 },    { T_WORD, "in", 1 },      { T_WORD, "in", 1 },
        { T_WORD, "in", 1 },    { T_WORD, "in", 1 },      { T_WORD, "in", 1 },
        { T_WORD, "in", 1 },   { T_WORD, "in", 0 }, { T_SEMICOLON, NULL, 1 }, { T_DO, NULL, 0 },
        { T_NEWLINE, NULL, 1 }, { T_WORD, "echo", 1 }, { T_WORD, "bonjour!", 0 },   { T_NEWLINE, NULL, 0 },
        { T_DONE, NULL, 0 }
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, REAL)
{
    char *script = "for i in 1 2 3; do\n echo $i\ndone";
    struct token_info expected[] = { { T_FOR, NULL, 1 },       { T_VAR, "i", 1 },
                                     { T_IN, NULL, 1 },        { T_WORD, "1", 1 },
                                     { T_WORD, "2", 1 },       { T_WORD, "3", 0 },
                                     { T_SEMICOLON, NULL, 1 }, { T_DO, NULL, 0 },
                                     { T_NEWLINE, NULL, 1 },   { T_WORD, "echo", 1 },
                                     { T_VAR, "i", 0 },        { T_NEWLINE, NULL, 0 },
                                     { T_DONE, NULL, 0} };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, WEIRD_VAR)
{
    char *script = "for $in in in; do\n  test\ndone";
    struct token_info expected[] = { { T_FOR, NULL, 1 },       { T_VAR, "$in", 1},
                                     { T_IN, NULL, 1 },        { T_WORD, "in", 0 },
                                     { T_SEMICOLON, NULL, 1 }, { T_DO, NULL, 0},
                                     { T_NEWLINE, NULL, 1 },   { T_WORD, "test", 0 },
                                     { T_NEWLINE, NULL, 0 },   { T_DONE, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, WEIRD)
{
    char *script = "for ; in in; do\n  test\ndone";
    struct token_info expected[] = { { T_FOR, NULL, 1 },       { T_VAR, ";", 1 },
                                     { T_IN, NULL, 1 },        { T_WORD, "in", 0 },
                                     { T_SEMICOLON, NULL, 1 }, { T_DO, NULL, 0 },
                                     { T_NEWLINE, NULL, 1 },   { T_WORD, "test", 0 },
                                     { T_NEWLINE, NULL, 0 },   { T_DONE, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, WEIRD2)
{
    char *script = "for test; do\n  test\ndone";
    struct token_info expected[] = { { T_FOR, NULL, 1 },       { T_VAR, "test", 0 },
                                     { T_SEMICOLON, NULL, 1 }, { T_DO, NULL, 0 },
                                     { T_NEWLINE, NULL, 1 },   { T_WORD, "test", 0 },
                                     { T_NEWLINE, NULL, 0 },   { T_DONE, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, WEIRD3)
{
    char *script = "for test; in do\n  test\ndone";
    struct token_info expected[] = { { T_FOR, NULL, 1 },       { T_VAR, "test", 0 },
                                     { T_SEMICOLON, NULL , 1}, { T_IN, NULL, 1 },
                                     { T_DO, NULL, 0 },        { T_NEWLINE, NULL, 1 },
                                     { T_WORD, "test", 0 },    { T_NEWLINE, NULL, 0 },
                                     { T_DONE, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, EASY)
{
    char *script = "echo $test";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_VAR, "test", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, EASY1)
{
    char *script = "echo $test salut";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_VAR, "test", 1 },
                                     { T_WORD, "salut", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, EASY2)
{
    char *script = "echo $";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "$", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, MEDIUM)
{
    char *script = "echo $(";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "$", 0 },
                                     { T_O_PRTH, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, MEDIUM2)
{
    char *script = "echo '$test'";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "$test", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, MEDIUM3)
{
    char *script = "echo $a$b";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_VAR, "a", 0 },
                                     { T_VAR, "b", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, MEDIUM_QUOTE)
{
    char *script = "echo '(\"'";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "(\"", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, HARDQUOTE)
{
    char *script = "echo $'a'";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "$a", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, WEIRD_FOR)
{
    char *script = "$for test in 1; do\n test\ndone";
    struct token_info expected[] = { { T_VAR, "for", 1 },      { T_WORD, "test", 1 },
                                     { T_WORD, "in", 1 },      { T_WORD, "1", 0 },
                                     { T_SEMICOLON, NULL, 1 }, { T_DO, NULL, 0 },
                                     { T_NEWLINE, NULL, 1 },   { T_WORD, "test", 0 },
                                     { T_NEWLINE, NULL, 0 },   { T_DONE, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, BRACKET)
{
    char *script = "echo ${im_a_var}";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_VAR, "im_a_var", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, EASY)
{
    char *script = "\\if";
    struct token_info expected[] = { { T_WORD, "if", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, EASY2)
{
    char *script = "echo \\;p";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, ";p", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, QUOTED)
{
    char *script = "echo '\\;&&'";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "\\;&&", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, BACKLINE)
{
    char *script = "echo test\\\nok";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "testok", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, MEDIUM)
{
    char *script = "\\;\\\n\\echo\\&&";
    struct token_info expected[] = { { T_WORD, ";echo&&", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, HARD)
{
    char *script = "echo \\\''c'\\\'";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "\'c\'", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, SIMPLE)
{
    char *script = "echo \"test\"";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "test", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, SIMPLE2)
{
    char *script = "echo \"test&&\"";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "test&&", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, SIMPLE3)
{
    char *script = "echo \"&&\"";
    struct token_info expected[] = { { T_WORD, "echo", 1 }, { T_WORD, "&&", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, MEDIUM)
{
    char *script = "echo \"$i < && > $test\"";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_VAR_INQUOTE, "i", 1 },
                                     { T_WORD, " < && > ", 0 },
                                     { T_VAR_INQUOTE, "test", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, MEDIUM2)
{
    char *script = "echo \"$i $test $test< && > $test\"";
    struct token_info expected[] = {
        { T_WORD, "echo", 1 },    { T_VAR_INQUOTE, "i", 1 },
        { T_WORD, " ", 0 },       { T_VAR_INQUOTE, "test", 1 },
        { T_WORD, " ", 0 },       { T_VAR_INQUOTE, "test", 0, },
        { T_WORD, "< && > ", 0 }, { T_VAR_INQUOTE, "test", 0 }
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, MEDIUM3)
{
    char *script = "echo \"Hello $world, if this is $working then is good ! \"";
    struct token_info expected[] = {
        { T_WORD, "echo", 1 },           { T_WORD, "Hello ", 0 },
        { T_VAR_INQUOTE, "world", 0 },   { T_WORD, ", if this is ", 0 },
        { T_VAR_INQUOTE, "working", 1 }, { T_WORD, " then is good ! ", 0 }
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, SPECIAL_VAR)
{
    char *script = "echo \"hello $$\"";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "hello ", 0 },
                                     { T_VAR_INQUOTE, "$", 0} };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, SPECIAL_VAR2)
{
    char *script = "echo \"hello $*\"";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "hello ", 0 },
                                     { T_VAR_INQUOTE, "*", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, SPECIAL_VAR)
{
    char *script = "echo hello $$";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "hello", 1 },
                                     { T_VAR, "$", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, ESCAPE)
{
    char *script = "echo \"ceci est une quote : \\\"\"";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "ceci est une quote : \"", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, ESCAPE)
{
    char *script = "echo \'ceci est une quote : \"\'";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "ceci est une quote : \"", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, ESCAPE2)
{
    char *script = "echo \'ceci est un escape : \\\'";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "ceci est un escape : \\", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, DOUBLE_QUOTE_MISSING)
{
    char *script = "echo \"test";
    struct token_info expected[] = { { T_WORD, "echo", 1 },
                                     { T_WORD, "test", 0 },
                                     { T_ERROR, NULL, 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, HARD2)
{
    char *script = "e\"c\"h\"o\"\"\"\"     ${test}\"";
    struct token_info expected[] = {
        { T_WORD, "echo     ", 0 },
        { T_VAR_INQUOTE, "test", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, DQUOTE)
{
    char *script = "echo \"\\test\"";
    struct token_info expected[] = {
        { T_WORD, "echo", 1 },
        { T_WORD, "\\test", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, DECLARATION)
{
    char *script = "test=salut";
    struct token_info expected[] = {
        { T_VAR_INIT, "test", 0 },
        { T_WORD, "salut", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, WITH_SEMI)
{
    char *script = "test=salut; echo $test";
    struct token_info expected[] = { { T_VAR_INIT, "test", 0 },
                                     { T_WORD, "salut", 0 },
                                     { T_SEMICOLON, NULL, 1 },
                                     { T_WORD, "echo", 1 },
                                     { T_VAR, "test", 0 } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, DECLARATION_QUOTE)
{
    char *script = "test=\"    salut\"";
    struct token_info expected[] = {
        { T_VAR_INIT, "test", 0 },
        { T_WORD, "    salut", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, BAD_DECLARATION_QUOTE)
{
    char *script = "te@t=te";
    struct token_info expected[] = {
        { T_WORD, "te@t=te", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, COLLE)
{
    char *script = "echo te$te";
    struct token_info expected[] = {
        { T_WORD, "echo", 1 },
        { T_WORD, "te", 0 },
        { T_VAR, "te", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, CONCAT_QUOTE)
{
    char *script = "echo \"$te$te\"";
    struct token_info expected[] = {
        { T_WORD, "echo", 1 },
        { T_VAR_INQUOTE, "te", 0 },
        { T_VAR_INQUOTE, "te", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, CONCAT_QUOTE2)
{
    char *script = "echo $te\"$te$te\"$te";
    struct token_info expected[] = {
        { T_WORD, "echo", 1 },      { T_VAR, "te", 0 }, { T_VAR_INQUOTE, "te", 0 },
        { T_VAR_INQUOTE, "te", 0 }, { T_VAR, "te", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, CONCAT_QUOTE3)
{
    char *script = "echo $te'$te$te'$te";
    struct token_info expected[] = {
        { T_WORD, "echo", 1 },
        { T_VAR, "te", 0 },
        { T_WORD, "$te$te", 0 },
        { T_VAR, "te", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, DUR)
{
    char *script = "1< test a=2 echo a=3";
    struct token_info expected[] = {
        { T_IONUMBER, "1", 0 }, { T_REDIR_I_1, NULL, 1 }, { T_WORD, "test", 1 },
        { T_VAR_INIT, "a", 0 }, { T_WORD, "2", 1 },  { T_WORD, "echo", 1 },
        { T_WORD, "a=3", 0 }
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(NOT, ECHO)
{
    char *script = "echo salut!";
    struct token_info expected[] = {
        { T_WORD, "echo", 1 },
        { T_WORD, "salut!", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, SPACES)
{
    char *script = "echo \"            salut j'ai plein d'espace dans mon      "
                   " echo       !!     \"";
    struct token_info expected[] = {
        { T_WORD, "echo", 1 },
        { T_WORD,
          "            salut j'ai plein d'espace dans mon       echo       !!  "
          "   ", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, SPACE_DECLARATION)
{
    char *script = "a= test!";
    struct token_info expected[] = {
        { T_VAR_INIT, "a", 1 },
        { T_WORD, "test!", 0 },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

/* int main()
{
      char *script = "test=salut";
     lexer_start(script, strlen(script));
     struct token_info tk;
     while ((tk = pop_token()).type != T_EOF)
         continue;
}*/
