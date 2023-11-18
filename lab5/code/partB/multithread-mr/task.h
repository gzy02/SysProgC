#ifndef TASK_H
#define TASK_H
#include "file-struct.h"
#include <stdlib.h>
// Exercise:
// Add your code here:

typedef struct Task_t
{
    int result_size;    // 输出文件的个数
    int task_size;      // 输入文件的个数
    const char **files; // 输入文件的名字
    fnode *node;
} task_t;

void Task_init(task_t *task, int task_size, const char **files, int result_size, const char *OUTPUT_DIR, const char *TMP_DIR);
void Task_destroy(task_t *task);

#endif