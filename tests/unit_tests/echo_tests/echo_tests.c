#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include "builtins.h"

void redirect_all_stdout(void)
{
    cr_redirect_stdout();
    cr_redirect_stderr();
}

Test(simple, exit_code, .init = redirect_all_stdout)
{
    echo("coucou");
    cr_assert_stdout_eq_str(("coucou\n"));
}

Test(endnewline, exit_code, .init = redirect_all_stdout)
{
    echo("-n coucou");
    cr_assert_stdout_eq_str(("coucou"));
}

Test(interpret, exit_code, .init = redirect_all_stdout)
{
    echo("-e co\\nuc\\tou");
    cr_assert_stdout_eq_str(("co\nuc\tou\n"));
}
Test(both, exit_code, .init = redirect_all_stdout)
{
    echo("-ne co\\nuc\\\\ou");
    cr_assert_stdout_eq_str(("co\nuc\\ou"));
}
Test(postflag, exit_code, .init = redirect_all_stdout)
{
    echo("-n c\\toucou -e");
    cr_assert_stdout_eq_str(("c\\toucou -e"));
}
Test(complexflags, exit_code, .init = redirect_all_stdout)
{
    echo("-n -nea       -ne \\tcoucou\\n test");
    cr_assert_stdout_eq_str(("-nea -ne \\tcoucou\\n test"));
}
Test(complexflags2, exit_code, .init = redirect_all_stdout)
{
    echo("-n -ne a       -ne \\tcoucou\\n t\\\\est");
    cr_assert_stdout_eq_str(("a -ne \tcoucou\n t\\est"));
}
