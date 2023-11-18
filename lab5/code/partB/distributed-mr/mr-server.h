#ifndef MR_SERVER_H
#define MR_SERVER_H
#include "../lib/concur-hash.h"
#include "file-struct.h"
#include <stdlib.h>
// Exercise:
// Add your code here:
typedef struct Reduce_t
{
    char *name;
    char **input_files;
} reduce_t;

typedef struct Task_t
{
    int result_size; // 输出文件的个数
    int task_size;   // 输入文件的个数
    fnode *node;     // 文件管理系统
    hash_t map_waiting, reduce_waiting, map_working, reduce_working;
} task_t;

void Task_destroy(task_t *task);

// Exercise:
// Add your code here:
char *task_get();
char *map_get();
void map_finish(char *filename, char **file_names);
char *reduce_get();
void reduce_finish(char *reduce_name);
int get_mapsize();
int get_reducesize();
void task_init(int file_size, char **array, int result_size);
#endif