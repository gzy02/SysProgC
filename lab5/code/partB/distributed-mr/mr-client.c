#include "../lib/list.h"
#include "../lib/map-reduce.h"
#include "../util/key-value.h"
#include "mr-rpcs.h"
#include "mr.h"
#include <assert.h>
#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
const int WAIT_TIME = 1;
const char *OUTPUT_DIR = "./output", *TMP_DIR = "/tmp/distri_data", *SEP = "\n";

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

void *work(void *arg)
{
    while (1)
    {
        char *task = strdup(Task_get());
        printf("\tTask: %s\n", task);
        if (strcmp(task, "FINISH") == 0)
        {
            pthread_exit(NULL);
        }
        else if (strcmp(task, "WAIT") == 0)
        {
            sleep(WAIT_TIME);
            continue;
        }
        else if (strcmp(task, "MAP") == 0)
        {
            char *filename = Map_get();
            if (filename == NULL || strcmp(filename, "") == 0)
            {
                // sleep(WAIT_TIME);
                continue;
            }
            // printf("\tfilename: %s\n", filename);
            int nOutput = Get_reducesize();
            char **files = map_phase(filename, nOutput);
            Map_finish(files, nOutput);
            // printf("\tmap finish\n");
        }
        else if (strcmp(task, "REDUCE") == 0)
        {
            char *reduce_name_list = Reduce_get();
            if (reduce_name_list == NULL || strcmp(reduce_name_list, "") == 0)
            {
                // sleep(WAIT_TIME);
                continue;
            }
            char *savecopy = strdup(reduce_name_list);
            int nInput = Get_mapsize();
            char *saveptr;
            char *reduce_name = strtok_r(reduce_name_list, SEP, &saveptr);
            char **reduce_list = (char **)malloc(sizeof(char *) * (nInput));
            for (int i = 0; i < nInput; ++i)
            {
                reduce_list[i] = strtok_r(NULL, SEP, &saveptr);
            }
            reduce_phase(reduce_name, reduce_list, nInput);
            Reduce_finish(savecopy);
            free(reduce_list);
            free(savecopy);
            // printf("\treduce finish\n");
        }
        else
        {
            perror("\ttask error\n");
            exit(1);
        }
        free(task);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: ./mr-client.out <libmr.so> <worker_size>\n");
        exit(0);
    }
    const char *libmr_so = argv[1];
    load_mr_library(libmr_so);
    // Exercise:
    // Add your code here:
    int worker_size = atoi(argv[2]);
    pthread_t workers[worker_size];
    for (int i = 0; i < worker_size; i++)
    {
        pthread_create(&workers[i], NULL, work, NULL);
    }
    for (int i = 0; i < worker_size; i++)
    {
        pthread_join(workers[i], NULL);
    }
    // work(NULL);
    return 0;
}