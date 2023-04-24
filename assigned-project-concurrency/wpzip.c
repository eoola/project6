#include <stdio.h>
#include <pthread.h>
#include <string.h>

struct ThreadInfo {
    int num;
    char result[50]; // might need to make this bigger, based on test cases
};

void* zip(void* arg) {
    //havent put zip code in yet, but this is what accepts the portion of the file its passed, then zips it.
    //at this point, the zipped file is given to result.
    pthread_exit(NULL);
}

int main() {
    pthread_t id[2]; //create two threads for now, so we know it works.
    char file; //at this point still need to add in the command line reader, so this is a placeholder for the file.
    char final[200] = ""; //This variable should be where the combined result is.
    for(int i = 0; i < 2; i++){ //again, the two will be replaced by the number of threads later
        pthread_create(&id[i], NULL, zip, file); //create each thread
    }
    //Making sure all threads finish.
    for(int i = 0; i < 2; i++){
        pthread_join(id[i], NULL);
    }

    for(int i = 0; i < 2; i++){
        strcat(final, id[i].result); //I broke something with pointers here, but this should put the file back together.
    }

    //Finally, whatever is needed to output in the way the test wants.
    return 0;
}