// seq-mr.c
#include "../lib/list.h"
#include "../lib/map-reduce.h"
#include "../util/key-value.h"
#include "file-struct.h"
#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

const char *TMP_DIR = "/tmp/";
const char *OUTPUT_DIR = "./output/";

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

static unsigned int djb_hash(const char *cp)
{
    unsigned hash = 5381;
    while (*cp)
        hash = 33 * hash ^ (unsigned char)*cp++;
    return hash;
}

// return all charaters in a file.
char *read_file_path(char *path)
{
    FILE *fp = fopen(path, "r+");
    if (fp == NULL)
    {
        perror("fopen read_file");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (stat(path, &st) != 0)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }
    long file_size = st.st_size;

    char *buf = malloc(file_size + 1);
    if (buf == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    fread(buf, 1, file_size, fp);
    buf[file_size] = '\0';

    fclose(fp);
    return buf;
}

// 传入文件名，自动在前面加上./../data/
char *read_file(char *file_name)
{
    char path[256];
    snprintf(path, sizeof(path), "./../data/%s", file_name);
    return read_file_path(path);
}

void map_phase(fnode *node, int nInput, int nOutputs)
{
    // map phase
    // Exercise:
    // Add your code here:
    for (int i = 0; i < nInput; ++i)
    {
        file_list cur_file_list = node->files[i];
        char *buf = read_file(cur_file_list.fname);
        list_t cur_kv_list = mr_map(buf); // 将文件内容读入内存，然后调用mr_map，统计词频，得到cur_kv_list
        free(buf);                        // 防止泄漏内存
        for (int i = 0; i < cur_kv_list->last; ++i)
        {
            kv_t cur_kv = cur_kv_list->containter[i];
            int map_id = djb_hash(cur_kv->key) % nOutputs;
            const char *output_filename = cur_file_list.inter->file_paths[map_id]; // 映射到的文件名
            FILE *fp = fopen(output_filename, "a");
            if (fp == NULL)
            {
                perror("fopen map_phase");
                exit(EXIT_FAILURE);
            }
            fprintf(fp, "%s %s\n", cur_kv->key, cur_kv->value); // 将k v写入文件
            fclose(fp);
        }
        list_destroy(cur_kv_list); // 防止泄漏内存
    }
}

#include "../lib/concur-hash.h"
void reduce_phase(fnode *node, int nInput, int nOutputs)
{
    // reduce phase
    // Exercise:
    // Add your code here:
    for (int j = 0; j < nOutputs; ++j)
    {
        char output_filename[256];
        snprintf(output_filename, sizeof(output_filename), "%s/%d.txt", node->output_dir, j + 1);
        FILE *fp = fopen(output_filename, "w");
        if (fp == NULL)
        {
            perror("fopen reduce_phase");
            exit(EXIT_FAILURE);
        }

        hash_t *hash_table = calloc(1, sizeof(*hash_table));
        hash_init(hash_table, 1024); // 1024个桶

        for (int i = 0; i < nInput; ++i)
        {
            char *buf = read_file_path(node->files[i].inter->file_paths[j]);
            char *saveptr_buf, *saveptr_line;
            char *line = strtok_r(buf, "\n", &saveptr_buf);
            while (line != NULL)
            {
                char *key = strtok_r(line, " ", &saveptr_line);
                char *value_str = strtok_r(NULL, " ", &saveptr_line);
                int value = atoi(value_str);

                // 处理键和值
                int *it = hash_lookup(hash_table, key);
                if (it == NULL)
                    hash_insert(hash_table, key, value);
                else
                    *it += value;

                line = strtok_r(NULL, "\n", &saveptr_buf);
            }
            free(buf); // 防止泄漏内存
        }
        // 遍历哈希表，写文件
        for (int i = 0; i < hash_table->num_buckets; ++i)
        {
            pthread_rwlock_rdlock(&hash_table->mu[i]); // 单线程情形下也可不加锁
            node_t *node = hash_table->buckets[i];
            while (node != NULL)
            {
                fprintf(fp, "%s %d\n", node->key, node->value);
                node = node->next;
            }
            pthread_rwlock_unlock(&hash_table->mu[i]);
        }
        // 销毁哈希表
        hash_destroy(hash_table);
        fclose(fp);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "Usage: seq-mr.out <nOutputs> <libmr.so> <file1> <file2> ... <fileN>\n");
        exit(0);
    }

    int nOutputs = atoi(argv[1]);
    const char *libmr_so = argv[2];
    load_mr_library(libmr_so);
    int nInput = argc - 3;

    printf("init_fnode\n");
    fnode *node = init_fnode(nInput, (const char **)argv + 2, nOutputs, TMP_DIR, OUTPUT_DIR);
    printf("map_phase\n");
    map_phase(node, nInput, nOutputs);
    printf("reduce_phase\n");
    reduce_phase(node, nInput, nOutputs);
    printf("free_fnode\n");
    free_fnode(node, nOutputs);

    return 0;
}