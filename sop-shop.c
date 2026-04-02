#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define SHOP_FILENAME "./shop"
#define MIN_SHELVES 8
#define MAX_SHELVES 256
#define MIN_WORKERS 1
#define MAX_WORKERS 64

#define ERR(source)                                     \
    do                                                  \
    {                                                   \
        fprintf(stderr, "%s:%d\n", __FILE__, __LINE__); \
        perror(source);                                 \
        kill(0, SIGKILL);                               \
        exit(EXIT_FAILURE);                             \
    } while (0)

typedef struct shop
{
    int* polki;
} shop_t;

typedef struct shared_mem
{
    pthread_mutex_t mutexy[MAX_SHELVES];
    int working;
    pthread_mutex_t work_mutex;
} shared_mem_t;

void usage(char* program_name)
{
    fprintf(stderr, "Usage: \n");
    fprintf(stderr, "\t%s n m\n", program_name);
    fprintf(stderr, "\t  n - number of items (shelves), %d <= n <= %d\n", MIN_SHELVES, MAX_SHELVES);
    fprintf(stderr, "\t  m - number of workers, %d <= m <= %d\n", MIN_WORKERS, MAX_WORKERS);
    exit(EXIT_FAILURE);
}

void ms_sleep(unsigned int milli)
{
    time_t sec = (int)(milli / 1000);
    milli = milli - (sec * 1000);
    struct timespec ts = {0};
    ts.tv_sec = sec;
    ts.tv_nsec = milli * 1000000L;
    if (nanosleep(&ts, &ts))
        ERR("nanosleep");
}

void swap(int* x, int* y)
{
    int tmp = *y;
    *y = *x;
    *x = tmp;
}

void shuffle(int* array, int n)
{
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        swap(&array[i], &array[j]);
    }
}

void print_array(int* array, int n)
{
    for (int i = 0; i < n; ++i)
    {
        printf("%3d ", array[i]);
    }
    printf("\n");
}

void child_work(int liczba_produktow, shop_t* shop, shared_mem_t* shared_mem)
{
    printf("[%d] Worker reports for a night shift.\n", getpid());
    srand(getpid());
    while (1)
    {
        pthread_mutex_lock(&shared_mem->work_mutex);
        if (!shared_mem->working)
        {
            pthread_mutex_unlock(&shared_mem->work_mutex);
            break;
        }
        pthread_mutex_unlock(&shared_mem->work_mutex);
        int n1 = 1 + rand() % liczba_produktow;
        int n2;
        do
            n2 = 1 + rand() % liczba_produktow;
        while (n2 == n1);

        if (n1 > n2)
        {
            swap(&n1, &n2);
        }
        pthread_mutex_lock(&shared_mem->mutexy[n1 - 1]);
        pthread_mutex_lock(&shared_mem->mutexy[n2 - 1]);
        if (shop->polki[n1 - 1] > shop->polki[n2 - 1])
        {
            ms_sleep(100);
            swap(&shop->polki[n1 - 1], &shop->polki[n2 - 1]);
        }
        pthread_mutex_unlock(&shared_mem->mutexy[n2 - 1]);
        pthread_mutex_unlock(&shared_mem->mutexy[n1 - 1]);
    }
}

void create_children(int liczba_pracownikow, int liczba_produktow, shop_t* shop, shared_mem_t* shared_mem)
{
    for (int i = 0; i < liczba_pracownikow; i++)
    {
        pid_t pid = fork();
        switch (pid)
        {
            case -1:
                ERR("fork");
                exit(EXIT_FAILURE);
            case 0:
                child_work(liczba_produktow, shop, shared_mem);
                exit(EXIT_SUCCESS);
        }
    }
}

shop_t open_shop(int n, shared_mem_t* shared_mem)
{
    int shop_size = n * sizeof(int);
    int shop_fd = open(SHOP_FILENAME, O_CREAT | O_RDWR | O_TRUNC, -1);
    if (shop_fd == -1)
        ERR("open");
    if (ftruncate(shop_fd, shop_size))
        ERR("ftruncate");
    shop_t shop;
    shop.polki = mmap(NULL, shop_size, PROT_READ | PROT_WRITE, MAP_SHARED, shop_fd, 0);
    if (shop.polki == MAP_FAILED)
        ERR("mmap");
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared_mem->work_mutex, &attr);
    shared_mem->working = 1;
    for (int i = 0; i < n; i++)
    {
        shop.polki[i] = i + 1;
        pthread_mutex_init(&shared_mem->mutexy[i], &attr);
    }

    if (close(shop_fd))
        ERR("close");
    return shop;
}

void create_manager(int liczba_pracownikow, int liczba_produktow, shop_t* shop, shared_mem_t* shared_mem)
{
    printf("[%d] Manager reports for a night shift.\n", getpid());

    while (1)
    {
        ms_sleep(500);
        msync(shop->polki, liczba_produktow * sizeof(int), MS_SYNC);
        for (int i = 0; i < liczba_produktow; i++)
        {
            pthread_mutex_lock(&shared_mem->mutexy[i]);
        }
        print_array(shop->polki, liczba_produktow);
        int is_sorted = 1;
        for (int i = 1; i < liczba_produktow; i++)
        {
            if (shop->polki[i - 1] > shop->polki[i])
            {
                is_sorted = 0;
                break;
            }
        }
        for (int i = 0; i < liczba_produktow; i++)
        {
            pthread_mutex_unlock(&shared_mem->mutexy[i]);
        }
        if (is_sorted)
        {
            printf("[%d] The shop shelves are sorted.\n", getpid());
            pthread_mutex_lock(&shared_mem->work_mutex);
            shared_mem->working = 0;
            pthread_mutex_unlock(&shared_mem->work_mutex);
            break;
        }
    }
}
int main(int argc, char** argv)
{
    // robic structy
    int liczba_produktow = atoi(argv[1]);
    int liczba_pracownikow = atoi(argv[2]);
    if (liczba_produktow < 8 || liczba_produktow > 256 || liczba_pracownikow < 1 || liczba_pracownikow > 64)
        usage(argv[0]);

    // int fd;
    // if((fd = open(SHOP_FILENAME, O_CREAT | O_RDWR | O_TRUNC, -1))== -1)
    //     ERR("open");

    // if(ftruncate(fd, liczba_produktow * sizeof(int)))
    //     ERR("ftruncate");
    // int* shop;
    // if((shop = (int*)mmap(NULL, sizeof(shop_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    //     ERR("mmap");
    shared_mem_t* shared_mem;

    if ((shared_mem = mmap(NULL, sizeof(shared_mem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) ==
        MAP_FAILED)
        ERR("mmap");

    shop_t shop = open_shop(liczba_produktow, shared_mem);

    shuffle(shop.polki, liczba_produktow);
    print_array(shop.polki, liczba_produktow);
    create_children(liczba_pracownikow, liczba_produktow, &shop, shared_mem);
    create_manager(liczba_pracownikow, liczba_produktow, &shop, shared_mem);

    while (wait(NULL) > 0)
    {
    }
    print_array(shop.polki, liczba_produktow);
    printf("Night shift in Bitronka is over\n");
    if (munmap(shop.polki, liczba_produktow * sizeof(int)))
        ERR("munmap");
    if (munmap(shared_mem, liczba_produktow * sizeof(int)))
        ERR("munmap");

    return EXIT_SUCCESS;
}
