#include "mr-server.h"
#include <stdlib.h>
#include <string.h>
// Exercise:
// Add your code here:

task_t *task; // 全局变量
const char *OUTPUT_DIR = "./output", *TMP_DIR = "/tmp/distri_data", *SEP = "\n";
const int num_buckets = 8; // 用来管理文件的hash表的桶数
// pthread_mutex_t task_mu;

void Task_destroy(task_t *task)
{
    printf("free_fnode\n");
    free_fnode(task->node, task->result_size);
}

void task_init(int file_size, char **array, int result_size)
{
    task = (task_t *)malloc(sizeof(task_t));
    hash_init(&task->map_waiting, num_buckets);
    hash_init(&task->reduce_waiting, num_buckets);
    hash_init(&task->map_working, num_buckets);
    hash_init(&task->reduce_working, num_buckets);

    task->task_size = file_size;
    task->result_size = result_size;
    task->node = init_fnode(file_size, (const char **)array, result_size, TMP_DIR, OUTPUT_DIR);

    for (int i = 0; i < file_size; i++)
    {
        hash_insert(&task->map_waiting, array[i], i);
    }
}

// worker的调度策略，先map再reduce
char *task_get()
{
    // pthread_mutex_lock(&task_mu);
    int map_waiting_size = hash_size(&task->map_waiting);
    int map_working_size = hash_size(&task->map_working);
    int reduce_waiting_size = hash_size(&task->reduce_waiting);
    int reduce_working_size = hash_size(&task->reduce_working);
    // pthread_mutex_unlock(&task_mu);

    if (map_waiting_size == 0 && map_working_size == 0 && reduce_waiting_size == 0 && reduce_working_size == 0)
        return "FINISH";
    else if (map_waiting_size != 0)
        return "MAP";
    else if (reduce_waiting_size != 0)
        return "REDUCE";
    else
        return "WAIT";
}

char *map_get()
{
    // pthread_mutex_lock(&task_mu);
    //  从map_waiting中取出一个文件名，放入map_working中
    node_t *node = hash_get_first(&task->map_waiting);
    if (node == NULL)
    {
        // pthread_mutex_unlock(&task_mu);
        return NULL;
    }

    char *filename = node->key;
    int id = node->value;
    free(node);

    hash_insert(&task->map_working, filename, id);
    // pthread_mutex_unlock(&task_mu);
    return filename;
}

void map_finish(char *filename, char **file_names)
{
    // pthread_mutex_lock(&task_mu);
    int id = *hash_lookup(&task->map_working, filename);
    if (hash_delete(&task->map_working, filename) == 1)
    {
        // pthread_mutex_unlock(&task_mu);
        perror("\tmap_finish delete error\n");
        return;
    }

    for (int i = 0; i < task->result_size; i++)
    {
        task->node->files[id].inter->file_paths[i] = strdup(file_names[i]);
    }

    // printf("\tmap_finish: %s\n", filename);

    // 如果map_waiting和map_working都为空，就将reduce_waiting填满
    if (hash_size(&task->map_waiting) == 0 && hash_size(&task->map_working) == 0)
    {
        // printf("\t\tmap_finish: fill reduce_waiting\n");

        for (int j = 0; j < task->result_size; j++)
        {
            // 生成reduce的文件名和要读取的文件（序列化）
            char buffer[1024];
            sprintf(buffer, "%s/%d.txt", task->node->output_dir, j);
            for (int i = 0; i < task->task_size; i++)
            {
                const char *target_path = task->node->files[i].inter->file_paths[j];
                strcat(buffer, SEP); // 序列化，用SEP分割
                strcat(buffer, target_path);
            }
            // 将文件名和文件内容写入reduce_waiting
            hash_insert(&task->reduce_waiting, strdup(buffer), j);
        }
        // printf("\t\tmap_finish: fill reduce_waiting finish %d\n", hash_size(&task->reduce_waiting));
    }
    // pthread_mutex_unlock(&task_mu);
}

char *reduce_get()
{
    // pthread_mutex_lock(&task_mu);
    //  从reduce_waiting中取出一个文件名，放入reduce_working中
    node_t *node = hash_get_first(&task->reduce_waiting);
    if (node == NULL)
    {
        // pthread_mutex_unlock(&task_mu);
        return NULL;
    }

    char *filename = node->key;
    int id = node->value;
    free(node);

    hash_insert(&task->reduce_working, filename, id);
    // // printf("\tinsert reduce_working: %s\n", filename);
    // pthread_mutex_unlock(&task_mu);
    return filename;
}

void reduce_finish(char *reduce_name)
{
    // pthread_mutex_lock(&task_mu);
    if (hash_delete(&task->reduce_working, reduce_name) == 1)
    {
        // pthread_mutex_unlock(&task_mu);
        perror("\treduce_finish delete error\n");
        return;
    }
    // pthread_mutex_unlock(&task_mu);
}

int get_mapsize()
{
    return task->task_size;
}
int get_reducesize()
{
    return task->result_size;
}
