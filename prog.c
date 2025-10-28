#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

#define MAX_BUFFER 256

void scan_dir(const char* path,const char * ext,FILE* fptr)
{
    DIR* dirp;
    struct dirent *dp;
    struct stat filestat;
    if((dirp=opendir("."))==NULL)
        ERR("opendir");
    if(fptr)
    {
        fprintf(fptr,"path: %s\n", path);
    }
    else {
        fprintf(stdout,"path: %s\n", path);
    }
    
    do {
    errno=0;
    if((dp=readdir(dirp))!=NULL)
    {
        if(lstat(dp->d_name, &filestat))
            ERR("lstat");
        if(ext !=NULL)
        {
            char*dot=strrchr(dp->d_name, '.');
            if(!dot || strcmp(dot+1,ext)!=0)
                continue;
        }
        if(fptr)
        {
           fprintf(fptr, "%s %lu\n",dp->d_name,filestat.st_size);
        }
        else {
            fprintf(stdout, "%s %lu\n",dp->d_name,filestat.st_size);
        }
        
    }
    }while (dp!=NULL);

    if(errno!=0)
        ERR("readdir");
    if(closedir(dirp))
        ERR("closedir");
}


int main(int argc, char** argv)
{
    int c,o_flag=0;
    char *path;
    char **paths = NULL;
    int npaths=0;
    char *extention=NULL;
    char *output=NULL;
    FILE *fptr=NULL;
    while((c=getopt(argc, argv, "p:e:o:"))!= -1)
    {
        switch (c) 
        {
            case 'e': 
            extention=optarg;
            break;
            case 'p':
            paths=realloc(paths, (npaths+1)*sizeof(char*));
            if(!paths) ERR("realloc");
            paths[npaths]=optarg;
            npaths++;
            break;
            case 'o':
            o_flag++;
            if(o_flag>1)
                ERR("o_flag");
            output=optarg;
            break;
            default:
            break;
        }
    }
    if(output)
    {
        fptr=fopen(output, "a");
        if(!fptr)
            ERR("fopen");
    }
    for(int i=0;i<npaths;i++)
    {

        if((path=getcwd(NULL, 0))==NULL)
                ERR("getcwd");
            if(chdir(paths[i]))
                ERR("chdir");
            scan_dir(paths[i],extention,fptr);
            if(chdir(path))
                ERR("chdir");
        free(path);    
    }
    free(paths);
    if(fptr && fclose(fptr))
        ERR("fclose");
    return EXIT_SUCCESS;
}