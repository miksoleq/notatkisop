#include <stdio.h>
#include <stdlib.h>
#define ERR(source) (perror(source),\
fprintf(stderr, "%s:%d\n", __FILE__, __LINE__),\
exit(EXIT_FAILURE))

int main(int argc, char **argv)
{
    int* a = malloc(sizeof(int));
    *a=10;
    printf("Hello world\n");
    printf("a = %d\n",*a);
    free(a);
    return EXIT_SUCCESS;
}
