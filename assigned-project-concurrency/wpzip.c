#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>

#define MAX_THREADS 16
#define QUEUE_SIZE 100

static char prev_char;
uint32_t count;
pthread_mutex_t mutex;
pthread_cond_t cond_var;
int lock_count = 0;

// Queue to hold file names
typedef struct
{
    char **files;
    int front, rear, size;
} FileQueue;

// Initialize the file queue
void init_file_queue(FileQueue *queue)
{
    queue->front = 0;
    queue->rear = -1;
    queue->size = 0;
    queue->files = malloc(sizeof(char *) * QUEUE_SIZE);
}

// Check if file queue is empty
int is_file_queue_empty(FileQueue *queue)
{
    return (queue->size == 0);
}

// Check if file queue is full
int is_file_queue_full(FileQueue *queue)
{
    return (queue->size == QUEUE_SIZE);
}

// Add a file name to the file queue
void enqueue_file(FileQueue *queue, char *file)
{
    if (!is_file_queue_full(queue))
    {
        queue->rear = (queue->rear + 1) % QUEUE_SIZE;
        queue->files[queue->rear] = file;
        queue->size++;
    }
}

// Remove a file name from the file queue
char *dequeue_file(FileQueue *queue)
{
    if (!is_file_queue_empty(queue))
    {
        char *file = queue->files[queue->front];
        queue->front = (queue->front + 1) % QUEUE_SIZE;
        queue->size--;
        return file;
    }
    return NULL;
}

// RLE compression function
void rle_compress(char *s, FILE *fp_out, char* buffer)
{
    char* moving_point = buffer;
    for (int i = 0; i < strlen(s); i++)
    {
        char cur_char = s[i];
        if (cur_char == prev_char)
        {
            // same character as previous one, increment count
            count++;
        }
        else
        {
            // different character, write out the previous count and char
            if (prev_char != '\0')
            {
                memcpy(moving_point, &count, sizeof(uint32_t));
                moving_point = moving_point + sizeof(uint32_t);
                memcpy(moving_point, &prev_char, sizeof(char));
                moving_point = moving_point + sizeof(char);
                //fwrite(&count, sizeof(uint32_t), 1, fp_out);
                //fwrite(&prev_char, sizeof(char), 1, fp_out);
            }
            // reset count for the new character
            count = 1;
            prev_char = cur_char;
        }
    }
}

// Worker thread function
void *compress_file(void *arg)
{
    FileQueue *queue = (FileQueue *)arg;
    char *file;

    while ((file = dequeue_file(queue)) != NULL)
    {
        FILE *fp_in = fopen(file, "r");
        if (fp_in == NULL)
        {
            fprintf(stderr, "Error: cannot open file '%s'\n", file);
            continue;
        }
        //Checking file size would most likely be here
        //Then make the buffer
        char buffer[1024] = "";
        char *line = NULL;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, fp_in)) != -1)
        {
            rle_compress(line, stdout, buffer);
        }

        // Increment counter while holding the mutex
        pthread_mutex_lock(&mutex);
        lock_count++;

        // Check if counter value matches expected value
        while (lock_count != id)
        {
            pthread_cond_wait(&cond_var, &mutex);
        }
        //Writing to file.
        fwrite(buffer, sizeof(buffer), 1, stdout);

        // Release the mutex and signal waiting threads
        pthread_mutex_unlock(&mutex);
        pthread_cond_broadcast(&cond_var);
        free(line);
        fclose(fp_in);
    }

    return NULL;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("wpzip: numOfThreads file1 [file2 ...]\n");
        exit(1);
    }

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0 || num_threads > MAX_THREADS)
    {
        num_threads = 1;
    }

    FileQueue file_queue;
    init_file_queue(&file_queue);

    // Enqueue all files
    for (int i = 2; i < argc; i++)
    {
        enqueue_file(&file_queue, argv[i]);
    }

    // Create and start worker threads
    pthread_t threads[num_threads];
    for (int i = 0; i < num_threads; i++)
    {
        if (pthread_create(&threads[i], NULL, compress_file, &file_queue) != 0)
        {
            fprintf(stderr, "Error: cannot create thread %d\n", i);
            exit(1);
        }
    }

    // Wait for all worker threads to finish
    for (int i = 0; i < num_threads; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            fprintf(stderr, "Error: cannot join thread %d\n", i);
            exit(1);
        }
    }

    // Flush the remaining compressed data
    if (prev_char != '\0')
    {
        fwrite(&count, sizeof(uint32_t), 1, stdout);
        fwrite(&prev_char, sizeof(char), 1, stdout);
    }

    // Cleanup
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_var);

    return 0;
}