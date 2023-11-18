#include "../lib/list.h"
#include "../lib/map-reduce.h"
#include "../util/key-value.h"
#include "mr.h"
#include "task.h"
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFF_SIZE 1024

task_t task; // task_t design in task.h

list_t (*mr_map)(char *);
char *(*mr_reduce)(char *, list_t);

void load_mr_library(const char *libmr_so)
{
    void *handle = dlopen(libmr_so, RTLD_LAZY);
    if (!handle)
    {
        fprintf(stderr, "Cannot open library: %s\n", dlerror());
        exit(1);
    }

    mr_map = dlsym(handle, "mr_map");
    if (!mr_map)
    {
        fprintf(stderr, "Cannot load symbol mr_map: %s\n", dlerror());
        exit(1);
    }

    mr_reduce = dlsym(handle, "mr_reduce");
    if (!mr_reduce)
    {
        fprintf(stderr, "Cannot load symbol mr_reduce: %s\n", dlerror());
        exit(1);
    }
}

void *work_map(void *arg)
{
    // Exercise:
    // Add your code here:
    long id = (long)arg;
    char *str = malloc(32);  // 为字符串分配足够的空间
    sprintf(str, "%ld", id); // 将long转换为字符串
    printf("map_id: %s\n", str);
    map_phase(&task, str);
    free(str); // 释放内存
    pthread_exit(0);
}

void *work_reduce(void *arg)
{
    // Exercise:
    // Add your code here:
    long id = (long)arg;
    char *str = malloc(32);  // 为字符串分配足够的空间
    sprintf(str, "%ld", id); // 将long转换为字符串
    printf("reduce_id: %s\n", str);
    reduce_phase(&task, str);
    free(str); // 释放内存
    pthread_exit(0);
}

int main(int argc, char *argv[])
{

    if (argc < 4)
    {
        fprintf(stderr, "Usage: mthd-mr.out <libmr.so> <result_size> <file1> <file2> ... <fileN>\n");
        exit(0);
    }
    const char *libmr_so = argv[1];
    load_mr_library(libmr_so);
    int result_size = atoi(argv[2]);
    int task_size = argc - 3;
    char **file_name = (char **)calloc(task_size, sizeof(char *));

    for (int i = 3; i < argc; i++)
        file_name[i - 3] = argv[i];

    const char *OUTPUT_DIR = "./output", *TMP_DIR = "/tmp/mth_data";

    // 更规范的创建文件夹
    remove_dir(OUTPUT_DIR);
    if (mkdir(OUTPUT_DIR, 0777) == -1)
    {
        perror("mkdir");
        exit(1);
    }
    remove_dir(TMP_DIR);
    if (mkdir(TMP_DIR, 0777) == -1)
    {
        perror("mkdir");
        exit(1);
    }

    Task_init(&task, task_size, (const char **)file_name, result_size, OUTPUT_DIR, TMP_DIR);
    free(file_name);
    pthread_t threads[BUFF_SIZE];

    printf("map_phase\n");
    for (long i = 0; i < task_size; i++)
        pthread_create(&threads[i], NULL, work_map, (void *)i);
    for (int i = 0; i < task_size; i++)
        pthread_join(threads[i], NULL);

    printf("reduce_phase\n");
    for (long i = 0; i < result_size; i++)
        pthread_create(&threads[i], NULL, work_reduce, (void *)i);
    for (int i = 0; i < result_size; i++)
        pthread_join(threads[i], NULL);

    Task_destroy(&task);
    return 0;
}