#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void usage(char *pname);
int main(int argc, char **argv)
{
    int x;
    //sscanf(argv[2],"%d", &x);
    x=atoi(argv[2]);
    if(argc!=3 || x<=0)
    {
        usage(argv[0]);
    }
    for(int i=0;i<x;i++)
    {
        printf("Hello %s\n", argv[1]);
    }
    return EXIT_SUCCESS;
}






void usage(char *pname)
{
  fprintf(stderr, "USAGE:%s name times>0\n", pname);
  exit(EXIT_FAILURE);
}
