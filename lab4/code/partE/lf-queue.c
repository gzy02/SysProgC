#include "lf-queue.h"
#include <stdio.h>
#include <stdlib.h>

void lf_queue_init(lf_queue_t *queue)
{
    node_t *node = (node_t *)calloc(1, sizeof(*node));
    atomic_store(&queue->front, node);
    atomic_store(&queue->rear, node);
    return;
}

void lf_queue_enqueue(lf_queue_t *queue, int value)
{
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    new_node->value = value;
    new_node->next = NULL;
    node_t *prev = atomic_exchange(&queue->rear, new_node);
    prev->next = new_node; // 最后再设置prev的next，保证prev的next不会被其他线程修改
    return;
}

int lf_queue_dequeue(lf_queue_t *queue)
{
    node_t *next;
    atomic_node head = queue->front;
    do
    {
        // 出队的时候最后剩下的是最后一次入队的节点
        next = head->next;
        if (next == NULL)
            return -1;
        // 比较head指针是否是我们之前获取的，成功则设置next，失败则自旋
    } while (!atomic_compare_exchange_weak(&queue->front, &head, next));
    free(head); // 释放dummy节点，此后出队的节点被视为dummy节点
    return next->value;
}

int lf_queue_size(lf_queue_t *queue)
{
    int size = 0;
    node_t *cur = atomic_load(&queue->front);
    while (cur != atomic_load(&queue->rear))
    {
        cur = cur->next;
        ++size;
    }
    return size;
}
