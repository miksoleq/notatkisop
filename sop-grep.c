#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))



void usage(int argc, char** argv)
{
    printf("%s pattern\n", argv[0]);
    printf("pattern - string pattern to search at standard input\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    char* pattern = argv[1];
    char* line=NULL;
    size_t line_len = 0;
    char *env = getenv("W1_LINENUMBER");
    char *env2 = getenv("W1_LOGFILE");
    FILE *fptr = NULL;
    if(env2 != NULL)
    {
        fptr=fopen(env2,"a");
        if(!fptr)
        {
            ERR("fopen W1_LOGFILE");
        }
        
    }
    if((env != NULL) && (strcmp(env,"1")==0 || strcmp(env, "TRUE")==0 || strcmp(env, "true")==0))
    {
        int x=1;
         while (getline(&line, &line_len, stdin) != -1)  // man 3p getdelim
        {
            if (strstr(line, pattern))
            {
                printf("%d:%s", x, line);  // getline() should return null terminated data
                fprintf(fptr, "%d:%s", x, line);
            }
            x++;
        }
    }
    else 
    {
        while (getline(&line, &line_len, stdin) != -1)  // man 3p getdelim
        {
            if (strstr(line, pattern))
            {
                printf("%s", line);  // getline() should return null terminated data
                fprintf(fptr, "%s", line);
            }
        }
    }

    if (line)
        free(line);
    if (fptr)
        fclose(fptr);

    return EXIT_SUCCESS;
}
