#include <stdio.h>
#include <pthread.h>

#define CHUNK_SIZE 1024

char uncompressed_data[CHUNK_SIZE];
int uncompressed_size = 0;

struct CompressedChunk
{
    int count;
    char value;
};

struct CompressedChunk compressed_data[CHUNK_SIZE];
int compressed_size = 0;

void *compress_chunk(void *arg)
{
    int *chunk_index = (int *)arg;

    int i;
    char current_char = uncompressed_data[(*chunk_index) * CHUNK_SIZE];
    int count = 1;
    for (i = 1; i < CHUNK_SIZE; i++)
    {
        char next_char = uncompressed_data[(*chunk_index) * CHUNK_SIZE + i];
        if (next_char == current_char)
        {
            count++;
        }
        else
        {
            compressed_data[(*chunk_index) * CHUNK_SIZE + compressed_size].count = count;
            compressed_data[(*chunk_index) * CHUNK_SIZE + compressed_size].value = current_char;
            compressed_size++;
            current_char = next_char;
            count = 1;
        }
    }

    compressed_data[(*chunk_index) * CHUNK_SIZE + compressed_size].count = count;
    compressed_data[(*chunk_index) * CHUNK_SIZE + compressed_size].value = current_char;
    compressed_size++;

    return NULL;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("wzip: numOfThreads file0 [file1...]");
        return 1;
    }

    FILE *input_file = fopen(argv[2], "rb");
    if (input_file == NULL)
    {
        printf("Error: could not open file %s\n", argv[1]);
        return 1;
    }

    int num_chunks = 1;
    fseek(input_file, 0, SEEK_END);
    int file_size = ftell(input_file);
    fseek(input_file, 0, SEEK_SET);
    if (file_size > CHUNK_SIZE)
    {
        num_chunks = file_size / CHUNK_SIZE + (file_size % CHUNK_SIZE != 0);
    }
    printf("Compressing file '%s' with %d threads and chunk size %d...\n", argv[1], num_chunks, CHUNK_SIZE);

    pthread_t threads[num_chunks];
    int thread_args[num_chunks];

    int i;
    for (i = 0; i < num_chunks; i++)
    {
        if (i == num_chunks - 1)
        {
            uncompressed_size = file_size % CHUNK_SIZE;
            if (uncompressed_size == 0)
            {
                uncompressed_size = CHUNK_SIZE;
            }
        }
        else
        {
            uncompressed_size = CHUNK_SIZE;
        }
        fread(uncompressed_data, 1, uncompressed_size, input_file);
        compressed_size = 0;
        thread_args[i] = i;
        pthread_create(&threads[i], NULL, compress_chunk, &thread_args[i]);
    }

    for (i = 0; i < num_chunks; i++)
    {
        pthread_join(threads[i], NULL);
    }

    for (i = 0; i < compressed_size * num_chunks; i++)
    {
        fwrite(&compressed_data[i].count, sizeof(int), 1, stdout);
        fwrite(&compressed_data[i].value, sizeof(char), 1, stdout);
    }

    fclose(input_file);

    return 0;
}
