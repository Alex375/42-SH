#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <xstrdup.h>
#include <stdlib.h>

void cd(char **args)
{
    if (args[1] == NULL)
        errx(1, "cd: Path is HOME");

    char *path = xstrdup(args[1]); // on recup le path passe en arg

    int buffer_size = 1024;
    char *current_path = calloc(1024, sizeof(char));
    while (getcwd(current_path, buffer_size) == NULL)
    {
        buffer_size += 1024;
        current_path = realloc(current_path, buffer_size);
    }
    current_path = getcwd(current_path, buffer_size);

    // print le current directory
    printf("You are ->%s\n", current_path);
    printf("Go to -> %s\n\n", path);
    chdir(path);
    char *new_path = calloc(1024, sizeof(char));
    new_path = getcwd(new_path, buffer_size);
    if (current_path == new_path)
        errx(1, "Pas possible to cd");

    printf("You are now ->%s\n", new_path);
}

int main()
{
    char *arr[] = { "bite", "bite"};
    printf("RUN TEST:\n\n");
    cd(arr);
}
