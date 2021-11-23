#include <lexer.h>
#include <criterion/criterion.h>
#include <string.h>

Test(SIMPLE_COMMAND, simple_case1)
{
    char* script = "echo pute; echo pute";
    struct token_info expected[] = {{T_COMMAND, "echo pute"}, {T_SEMICOLON, NULL}, {T_COMMAND, "echo pute"}};
    struct token_info actual_value;
    size_t size = 3;

    for (size_t i = 0; i < size; i++)
    {
        actual_value = pop_token(script, strlen(script));
        cr_assert_eq(actual_value.type, expected[i].type, "Expected: %d. Got: %d"
                     , expected[i].type, actual_value.type);
    }
}
