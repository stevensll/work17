#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#define KEY 24601
#define BUFF_LEN 1000

int main(){
    //GET SEMAPHORE AND PERFORM OPERATION
    int semd = semget(KEY, 1, 0);
    if(semd < 0) printf("Error getting semaphore: %s", strerror(errno));
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_flg = SEM_UNDO;
    sb.sem_op = -1;
    semop(semd, &sb, 1);

    //GET SHARED MEMORY AND GET LEN
    int shmd = shmget(KEY, sizeof(int), 0);
    if(shmd < 0) printf("Error getting shared memory: %s", strerror(errno));
    int *len = shmat(shmd, 0, 0);
    if (*len < 0) printf("Error finding length associated with shared memory: %s", strerror(errno));

    //OPEN FILE
    int fd = open("f.txt", O_RDWR | O_APPEND);
    if(fd < 0) printf("Error opening file: %s", strerror(errno));
    
    //FIND END AND READ
    char *buff = calloc(BUFF_LEN, sizeof(char));
    lseek(fd,-(*len), SEEK_END);
    read(fd, buff, BUFF_LEN);
    printf("Current line is : %s\n", buff);
    printf("Input new line with at most %d characters:\n", BUFF_LEN);
    fgets(buff, BUFF_LEN, stdin);
    lseek(fd, 0, SEEK_END);
    write(fd, buff, strlen(buff));
    *len = strlen(buff);
    printf("Wrote to file\n");


    //UPDATE SHMD
    shmdt(len);
    //RELEASE SEMAPHORE
    sb.sem_op = 1;
    semop(semd, &sb, 1);
    return 0;
}