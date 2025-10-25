#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 20

int main(int argc, char **argv)
{
    char name[MAX_LINE + 2];
    char *times=getenv("TIMES");
    int x;
    if(times)
    {
        x= atoi(times); 
    }
    else {
         x=1;
    }

    while (fgets(name, MAX_LINE + 2, stdin) != NULL)
    {
        //name[strcspn(name, "\n")] = '\0';
        for(int j=0;j<x;j++)
        printf("Hello %s", name);
    }
    if(putenv("RESULT=Done")!=0)
    {
        fprintf(stderr, "putenv failed");
        return EXIT_FAILURE;;
    }
    printf("%s\n",getenv("RESULT"));
    if(system("env|grep RESULT")!=0) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
