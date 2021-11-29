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

Test(PARENTHESIS, SIMPLE_LS)
{
    char *script = "(ls -la)";
    struct token_info expected[] = { { T_O_PRTH, NULL },
                                     { T_WORD, "ls" },
                                     { T_WORD, "-la" },
                                     { T_C_PRTH, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(COMMAND, SIMPLE)
{
    char *script = "echo test\n";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_NEWLINE, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(COMMAND, MEDIUM)
{
    char *script = "echo if \n";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "if" },
                                     { T_NEWLINE, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(COMMAND, SIMPLE_LS)
{
    char *script = "ls -la";
    struct token_info expected[] = { { T_WORD, "ls" }, { T_WORD, "-la" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(COMMAND, LIST)
{
    char *script = "echo test; echo test";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_SEMICOLON, NULL },
                                     { T_WORD, "echo" },
                                     { T_WORD, "test" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(PARENTHIS, COMMAND_LIST)
{
    char *script = "echo test; (echo test)";
    struct token_info expected[] = { { T_WORD, "echo" },    { T_WORD, "test" },
                                     { T_SEMICOLON, NULL }, { T_O_PRTH, NULL },
                                     { T_WORD, "echo" },    { T_WORD, "test" },
                                     { T_C_PRTH, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(PARENTHESIS, DOUBLE)
{
    char *script = "echo test; ((echo test))";
    struct token_info expected[] = { { T_WORD, "echo" },    { T_WORD, "test" },
                                     { T_SEMICOLON, NULL }, { T_O_PRTH, NULL },
                                     { T_O_PRTH, NULL },    { T_WORD, "echo" },
                                     { T_WORD, "test" },    { T_C_PRTH, NULL },
                                     { T_C_PRTH, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(IF, SIMPLE)
{
    char *script = "if true; then\n echo test\n fi";
    struct token_info expected[] = { { T_IF, NULL },        { T_WORD, "true" },
                                     { T_SEMICOLON, NULL }, { T_THEN, NULL },
                                     { T_NEWLINE, NULL },   { T_WORD, "echo" },
                                     { T_WORD, "test" },    { T_NEWLINE, NULL },
                                     { T_FI, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(IF, WRONG)
{
    char *script = "ifecho true; then\n echo test\n fi";
    struct token_info expected[] = { { T_WORD, "ifecho" },  { T_WORD, "true" },
                                     { T_SEMICOLON, NULL }, { T_THEN, NULL },
                                     { T_NEWLINE, NULL },   { T_WORD, "echo" },
                                     { T_WORD, "test" },    { T_NEWLINE, NULL },
                                     { T_FI, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
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

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, EASY)
{
    char *script = "echo '&&'";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, "&&" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, MEDIUM)
{
    char *script = "echo      '      pute'";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "      pute" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, EMPTY)
{
    char *script = "echo  ''''";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, "" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, HARD)
{
    char *script = " i'f''&&'&&'ec'ho '%% pute'";
    struct token_info expected[] = { { T_WORD, "if&&" },
                                     { T_AND, NULL },
                                     { T_WORD, "echo" },
                                     { T_WORD, "%% pute" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(QUOTE, ERROR)
{
    char *script = "echo ''cest une erreur'";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "cest" },
                                     { T_WORD, "une" },
                                     { T_ERROR, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, EASY)
{
    char *script = "echo test>test.txt";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_REDIR_1, NULL },
                                     { T_WORD, "test.txt" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, EASY2)
{
    char *script = "echo test<test.txt";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_REDIR_I_1, NULL },
                                     { T_WORD, "test.txt" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM)
{
    char *script = "echo test>&test.txt";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_REDIR_A, NULL },
                                     { T_WORD, "test.txt" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM2)
{
    char *script = "echo test<&test.txt";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_REDIR_I_A, NULL },
                                     { T_WORD, "test.txt" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM3)
{
    char *script = "echo test>>test.txt";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_REDIR_2, NULL },
                                     { T_WORD, "test.txt" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM4)
{
    char *script = "echo test<>test.txt";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_REDIR_O_2, NULL },
                                     { T_WORD, "test.txt" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM5)
{
    char *script = "echo test>|test.txt";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_REDIR_PIPE, NULL },
                                     { T_WORD, "test.txt" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, LONG)
{
    char *script = "echo test 1023>test.txt ";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_IONUMBER, "1023" },
                                     { T_REDIR_1, NULL },
                                     { T_WORD, "test.txt" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM_io)
{
    char *script = "echo test 2>|test.txt";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_IONUMBER, "2" },
                                     { T_REDIR_PIPE, NULL },
                                     { T_WORD, "test.txt" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM2_io)
{
    char *script = "echo test 2<test.txt";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_IONUMBER, "2" },
                                     { T_REDIR_I_1, NULL },
                                     { T_WORD, "test.txt" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, MEDIUM2_WRONG_IO)
{
    char *script = "echo test caca<test.txt";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test" },
                                     { T_WORD, "caca" },
                                     { T_REDIR_I_1, NULL },
                                     { T_WORD, "test.txt" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(REDIR, HARD)
{
    char *script = "echo test88> test.txt";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "test88" },
                                     { T_REDIR_1, NULL },
                                     { T_WORD, "test.txt" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(OR, EASY)
{
    char *script = "echo test || echo test";
    struct token_info expected[] = {
        { T_WORD, "echo" }, { T_WORD, "test" }, { T_OR, NULL },
        { T_WORD, "echo" }, { T_WORD, "test" },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(OR, MEDIUM)
{
    char *script = "echo test||echo test";
    struct token_info expected[] = {
        { T_WORD, "echo" }, { T_WORD, "test" }, { T_OR, NULL },
        { T_WORD, "echo" }, { T_WORD, "test" },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(AND, MEDIUM)
{
    char *script = "echo test&&echo test";
    struct token_info expected[] = {
        { T_WORD, "echo" }, { T_WORD, "test" }, { T_AND, NULL },
        { T_WORD, "echo" }, { T_WORD, "test" },
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(WHILE, EASY)
{
    char *script = "while true\ndo\necho caca\ndone";
    struct token_info expected[] = { { T_WHILE, NULL },   { T_WORD, "true" },
                                     { T_NEWLINE, NULL }, { T_DO, NULL },
                                     { T_NEWLINE, NULL }, { T_WORD, "echo" },
                                     { T_WORD, "caca" },  { T_NEWLINE, NULL },
                                     { T_DONE, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(UNTIL, EASY)
{
    char *script = "until true\ndo\necho caca\ndone";
    struct token_info expected[] = { { T_UNTIL, NULL },   { T_WORD, "true" },
                                     { T_NEWLINE, NULL }, { T_DO, NULL },
                                     { T_NEWLINE, NULL }, { T_WORD, "echo" },
                                     { T_WORD, "caca" },  { T_NEWLINE, NULL },
                                     { T_DONE, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(COMMAND, SPACES_AFTER)
{
    char *script = "echo coucou ";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, "coucou" } };

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
    struct token_info expected[] = { { T_WORD, "" } };

    test_lexer(script, 1, expected);
}

Test(FOR, EASY)
{
    char *script = "for test in 1 2 3\ndo\n echo 'jadore h'\ndone";
    struct token_info expected[] = {
        { T_FOR, NULL },     { T_VAR, "test" },      { T_IN, NULL },
        { T_WORD, "1" },     { T_WORD, "2" },        { T_WORD, "3" },
        { T_NEWLINE, NULL }, { T_DO, NULL },         { T_NEWLINE, NULL },
        { T_WORD, "echo" },  { T_WORD, "jadore h" }, { T_NEWLINE, NULL },
        { T_DONE, NULL }
    };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, MEDIUM)
{
    char *script = "for in in in; do\n  test\ndone";
    struct token_info expected[] = { { T_FOR, NULL },       { T_VAR, "in" },
                                     { T_IN, NULL },        { T_WORD, "in" },
                                     { T_SEMICOLON, NULL }, { T_DO, NULL },
                                     { T_NEWLINE, NULL },   { T_WORD, "test" },
                                     { T_NEWLINE, NULL },   { T_DONE, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, REAL)
{
    char *script = "for i in 1 2 3; do\n echo $i\ndone";
    struct token_info expected[] = { { T_FOR, NULL },       { T_VAR, "i" },
                                     { T_IN, NULL },        { T_WORD, "1" },
                                     { T_WORD, "2" },       { T_WORD, "3" },
                                     { T_SEMICOLON, NULL }, { T_DO, NULL },
                                     { T_NEWLINE, NULL },   { T_WORD, "echo" },
                                     { T_VAR, "i" },        { T_NEWLINE, NULL },
                                     { T_DONE, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, WEIRD_VAR)
{
    char *script = "for $in in in; do\n  test\ndone";
    struct token_info expected[] = { { T_FOR, NULL },       { T_VAR, "$in" },
                                     { T_IN, NULL },        { T_WORD, "in" },
                                     { T_SEMICOLON, NULL }, { T_DO, NULL },
                                     { T_NEWLINE, NULL },   { T_WORD, "test" },
                                     { T_NEWLINE, NULL },   { T_DONE, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, WEIRD)
{
    char *script = "for ; in in; do\n  test\ndone";
    struct token_info expected[] = { { T_FOR, NULL },       { T_VAR, ";" },
                                     { T_IN, NULL },        { T_WORD, "in" },
                                     { T_SEMICOLON, NULL }, { T_DO, NULL },
                                     { T_NEWLINE, NULL },   { T_WORD, "test" },
                                     { T_NEWLINE, NULL },   { T_DONE, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, WEIRD2)
{
    char *script = "for test; do\n  test\ndone";
    struct token_info expected[] = { { T_FOR, NULL },       { T_VAR, "test" },
                                     { T_SEMICOLON, NULL }, { T_DO, NULL },
                                     { T_NEWLINE, NULL },   { T_WORD, "test" },
                                     { T_NEWLINE, NULL },   { T_DONE, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(FOR, WEIRD3)
{
    char *script = "for test; in do\n  test\ndone";
    struct token_info expected[] = { { T_FOR, NULL },       { T_VAR, "test" },
                                     { T_SEMICOLON, NULL }, { T_IN, NULL },
                                     { T_DO, NULL },        { T_NEWLINE, NULL },
                                     { T_WORD, "test" },    { T_NEWLINE, NULL },
                                     { T_DONE, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, EASY)
{
    char *script = "echo $test";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_VAR, "test" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, EASY1)
{
    char *script = "echo $test salut";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_VAR, "test" },
                                     { T_WORD, "salut" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, EASY2)
{
    char *script = "echo $";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, "$" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, MEDIUM)
{
    char *script = "echo $(";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_WORD, "$" },
                                     { T_O_PRTH, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, MEDIUM2)
{
    char *script = "echo '$test'";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, "$test" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, MEDIUM3)
{
    char *script = "echo $a$b";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_VAR, "$a" }, { T_VAR, "$b" }};

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, MEDIUM_QUOTE)
{
    char *script = "echo '(\"'";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, "(\"" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, WEIRD_FOR)
{
    char *script = "$for test in 1; do\n test\ndone";
    struct token_info expected[] = { { T_VAR, "for" },      { T_WORD, "test" },
                                     { T_WORD, "in" },      { T_WORD, "1" },
                                     { T_SEMICOLON, NULL }, { T_DO, NULL },
                                     { T_NEWLINE, NULL },   { T_WORD, "test" },
                                     { T_NEWLINE, NULL },   { T_DONE, NULL } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(VAR, BRACKET)
{
    char *script = "echo ${im_a_var}";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_VAR, "im_a_var" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, EASY)
{
    char *script = "\\if";
    struct token_info expected[] = { { T_WORD, "if" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, EASY2)
{
    char *script = "echo \\;p";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, ";p" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, QUOTED)
{
    char *script = "echo '\\;&&'";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, "\\;&&" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, BACKLINE)
{
    char *script = "echo test\\\nok";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, "testok" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, MEDIUM)
{
    char *script = "\\;\\\n\\echo\\&&";
    struct token_info expected[] = { { T_WORD, ";echo&&" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(ESCAPE, HARD)
{
    char *script = "echo \\\''c'\\\'";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, "\'c\'" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, SIMPLE)
{
    char *script = "echo \"test\"";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, "test" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, SIMPLE2)
{
    char *script = "echo \"test&&\"";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, "test&&" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, SIMPLE3)
{
    char *script = "echo \"&&\"";
    struct token_info expected[] = { { T_WORD, "echo" }, { T_WORD, "&&" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, MEDIUM)
{
    char *script = "echo \"$i < && > $test";
    struct token_info expected[] = { { T_WORD, "echo" },
                                     { T_VAR, "i" },
                                     { T_WORD, " < && > " },
                                     { T_VAR, "test" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

Test(DOUBLE_QUOTE, MEDIUM2)
{
    char *script = "echo \"$i $test $test< && > $test";
    struct token_info expected[] = { { T_WORD, "echo" },   { T_VAR, "i" },
                                     { T_WORD, " " },      { T_VAR, "test" },
                                     { T_WORD, " " },      { T_VAR, "test<" },
                                     { T_WORD, " && > " }, { T_VAR, "test" } };

    test_lexer(script, EXPECTED_SIZE(expected), expected);
}

// int main()
//{
//     char *script = "echo \"$i $test $test< && > $test";
//     lexer_start(script, strlen(script));
//     struct token_info tk;
//     while ((tk = pop_token()).type != T_EOF)
//         continue;
// }
