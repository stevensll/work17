#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <fcntl.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

#define KEY 24601
#define BUFF_LEN 1000

union semun {
  int val;                  //used for SETVAL
  struct semid_ds *buf;     //used for IPC_STAT and IPC_SET
  unsigned short  *array;   //used for SETALL
  struct seminfo  *__buf;
};

void create(){
    //SEMAPHORE
    int semd = semget(KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
    if (semd < 0) printf("Error occured creating semaphore: %s\n", strerror(errno));
    printf("Created semaphore\n");

    //SETTING SEMAPHORE
    union semun us;
    us.val = 1;
    int r = semctl(semd, 0, SETVAL, us);
    if (r < 0) printf("Error occured setting value of semaphore: %s\n", strerror(errno));
    printf("Set semaphore\n");

    //CREATING SHARED MEMORY
    int shmd = shmget(KEY, sizeof(int), IPC_CREAT | 0600);
    if (shmd < 0) printf("Error occured creating shared memory: %s\n", strerror(errno));
    int *data = shmat(shmd, 0, 0);
    *(data) = 0;
    printf("Created shared memory\n");

    //FILE
    int fd = open("f.txt", O_CREAT | O_TRUNC, 0644);
    if (fd < 0) printf("Error occured opening file: %s\n", strerror(errno));
    printf("Opened file\n");
    close(fd);
}

void rem(){
    int semd = semget(KEY, 1, 0);
    if (semd < 0) printf("Error occured getting semaphore: %s\n", strerror(errno));
    //REMOVE SEMAPHORE
    int s = semctl(semd, IPC_RMID, 0);
    if (s < 0) printf("Error removing semaphore: %s\n", strerror(errno));
    printf("Semaphore deleted\n");

    int shmd = shmget(KEY, 0, 0);
    if (shmd < 0) printf("Error occured getting shared memory: %s\n", strerror(errno));
    int r = shmctl(shmd, IPC_RMID, 0);
    if (r < 0) printf("Error removing shared memory: %s\n", strerror(errno));
    printf("Shared memory deleted\n");

    char buff[BUFF_LEN];
    int fd = open("f.txt", O_RDONLY);
    int x = read(fd, buff, BUFF_LEN);
    buff[x] = '\0';
    printf("Story:\n%s\n", buff);    
}

int main(int argc, char *argv[]){
    printf("Usage: Provide argument 'create' or 'remove' without quotation marks.\n");
    if (argc == 2 && strcmp(argv[1], "create") == 0){
        printf("Creating semaphore, shared memory, and opening file.\n");
        create();
    }
    else if(argc == 2 && strcmp(argv[1], "remove") ==0){
        printf("Deleting semaphore, shared memory, and reading file.\n");
        rem();
    }

    return 0;
}