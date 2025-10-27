// headery
#define _XOPEN_SOURCE 700
#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ftw.h>
#include <errno.h>
#include <getopt.h>

// makra
#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))
#define ERR_NOEXIT(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__))
#define FREE(ptr) (free(ptr), ptr = NULL)

// usage 
void usage(char *pname) {
    fprintf(stderr, "USAGE:%s [usage]\n", pname);
    exit(EXIT_FAILURE);
}
int main(int argc, char **argv) {
// skanowanie katalogu
DIR *dirp = opendir(".");
if (dirp == NULL)
    ERR("opendir");
struct dirent *entp = NULL; // pointer do kolejnych elementow katalogu
struct stat statbuf; // zalokowana struktura dla kolejnych stat()
while (1) {
    errno = 0;
    entp = readdir(dirp);
    if (entp == NULL) {
            if (errno)
                ERR("readdir"); // blad w readdir
            else break; // koniec katalogu
        }
    if (lstat(entp->d_name, &statbuf) == -1)
        ERR("lstat");
    if (stat(entp->d_name, &statbuf) == -1)
        ERR("stat");
    if (fstatat(dirfd(dirp), entp->d_name, &statbuf, AT_SYMLINK_NOFOLLOW) == -1) // AT_SYMLINK_NOFOLLOW - symlink to plik (POCZYTAC O FLAGACH)
        ERR("fstatat");
    // statbuf.st_*
}
// nftw - rekurencyjne przechodzenie po katalogu
int walk(const char *name, const struct stat *s, int type, struct FTW *f); // dla sukcesu zwraca 0
if(nftw(".", walk, 20, FTW_PHYS) == -1) // FTW_PHYS - nie podazaj za linkami (POCZYTAC O FLAGACH)
    ERR_NOEXIT("nftw");
// parsowanie parametrow (w tym parametr wielokrotny)
int c;
char **paths = malloc(argc*sizeof(char*)); // bedzie < argc stringow
if (paths == NULL)
    ERR("malloc");
int pathc = 0; // ilosc wczytanych stringow
int d = 0;
while ((c = getopt(argc, argv, "p:d:")) != -1) {
    switch (c) {
        case 'p':
            paths[pathc] = optarg; // nie trzeba alokowac bo optarg zwraca pointer do argv!!
            pathc++;
            break;
        case 'd':
            d = atoi(optarg); // atoi zwraca 0 dla bledu (mozna uzyc strtol)
            if (d < 1)
                usage(argv[0]);
            break;
        case '?':
        default:
            usage(argv[0]);
    }
}
FREE(paths); // po uzyciu trzeba zwolnic tylko tablice wskaznikow bo one wskazuja na argv
// katalogi tez trzeba zamykac!! W przyp. bledu closedir zwraca -1 a fclose EOF
if (closedir(dirp) == -1)
    ERR("closedir");
}
