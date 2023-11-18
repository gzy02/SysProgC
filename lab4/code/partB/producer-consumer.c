#include <pthread.h>
#include <stdio.h>

#define N 100
#define NUM_PRODUCERS 3
#define NUM_CONSUMERS 5

struct buffer_t
{
    pthread_mutex_t mutex;
    pthread_cond_t cond_full;
    pthread_cond_t cond_empty;
    long data[N];
    long size;
    long write_index;
    long read_index;
};

struct buffer_t buffer;

void initialize_buffer(struct buffer_t *buf)
{
    pthread_mutex_init(&buf->mutex, NULL);
    pthread_cond_init(&buf->cond_full, NULL);
    pthread_cond_init(&buf->cond_empty, NULL);
}

long production_id = 0;

void *producer(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&buffer.mutex);
        while ((buffer.write_index + 1) % N == buffer.read_index) // full
        {
            pthread_cond_wait(&buffer.cond_full, &buffer.mutex);
        }
        buffer.data[buffer.write_index] = production_id;
        printf("Producer. ID = %ld, position = %ld\n", production_id, buffer.write_index);
        buffer.write_index = (buffer.write_index + 1) % N;
        buffer.size++;
        production_id++;
        pthread_cond_signal(&buffer.cond_empty); // signal consumer
        pthread_mutex_unlock(&buffer.mutex);
    }
    pthread_exit(0);
}

void *consumer(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&buffer.mutex);
        while (buffer.write_index == buffer.read_index) // empty
        {
            pthread_cond_wait(&buffer.cond_empty, &buffer.mutex);
        }
        long consumption_id = buffer.data[buffer.read_index];
        printf("\tConsumer. ID = %ld, position = %ld\n", consumption_id, buffer.read_index);
        buffer.read_index = (buffer.read_index + 1) % N;
        buffer.size--;
        pthread_cond_signal(&buffer.cond_full); // signal producer
        pthread_mutex_unlock(&buffer.mutex);
    }
    pthread_exit(0);
}

int main()
{
    initialize_buffer(&buffer);

    pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS];

    for (long i = 0; i < NUM_PRODUCERS; ++i)
    {
        pthread_create(&producers[i], NULL, producer, 0);
    }
    for (long i = 0; i < NUM_CONSUMERS; ++i)
    {
        pthread_create(&consumers[i], NULL, consumer, 0);
    }
    for (long i = 0; i < NUM_PRODUCERS; ++i)
    {
        pthread_join(producers[i], NULL);
    }
    for (long i = 0; i < NUM_CONSUMERS; ++i)
    {
        pthread_join(consumers[i], NULL);
    }
    pthread_mutex_destroy(&buffer.mutex);
    pthread_cond_destroy(&buffer.cond_full);
    pthread_cond_destroy(&buffer.cond_empty);

    pthread_exit(0);
}
