#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char **argv)
{
    int index = 0;
    while (environ[index])
        printf("%s\n", environ[index++]);
    char* dom= getenv("HOME");
    printf("Katalog domowy to: %s\n", dom);
    return EXIT_SUCCESS;
}