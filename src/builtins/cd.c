#include <unistd.h>

void cd(char **args)
{
    if (args[1] == NULL)
        errx(1, "cd: Path is HOME");

    char test_buffer[100];

    char *path;
    strcpy(path, args[1]); // on recup le path passe en arg

    // print le current directory
    printf("%s\n", getcwd(test_buffer, 100));
    chdir(path);
    printf(test_buffer, 100);
}

int main()
{
    printf("coucou\n");
}