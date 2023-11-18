#include "task.h"

// Exercise:
// Add your code here:
void Task_init(task_t *task, int task_size, const char **files, int result_size, const char *OUTPUT_DIR, const char *TMP_DIR)
{
    task->task_size = task_size;
    task->files = files;
    task->result_size = result_size;

    printf("init_fnode\n");
    task->node = init_fnode(task_size, files, result_size, TMP_DIR, OUTPUT_DIR);
}

void Task_destroy(task_t *task)
{
    printf("free_fnode\n");
    free_fnode(task->node, task->result_size);
}