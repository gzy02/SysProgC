#include "../lib/concur-hash.h"
#include "../lib/list.h"
#include "../lib/map-reduce.h"
#include "../util/key-value.h"
#include "file-struct.h"

#include "task.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

extern list_t (*mr_map)(char *);
extern char *(*mr_reduce)(char *, list_t);

// hash
/* this is the D. J. Bernstein hash function */
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

// map_name是要处理的文件id
void map_phase(task_t *task, char *map_name)
{
    // map phase
    // Exercise:
    // Add your code here:
    fnode *node = task->node;
    int nOutputs = task->result_size;
    int map_id = atoi(map_name);
    file_list cur_file_list = node->files[map_id];
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

// reduce_name是要写入的文件id
void reduce_phase(task_t *task, char *reduce_name)
{
    // reduce phase
    // Exercise:
    // Add your code here:
    fnode *node = task->node;
    int nInput = task->task_size;
    char output_filename[256];
    int reduce_id = atoi(reduce_name);
    snprintf(output_filename, sizeof(output_filename), "%s/%d.txt", node->output_dir, reduce_id + 1);
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
        char *buf = read_file_path(node->files[i].inter->file_paths[reduce_id]);
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
