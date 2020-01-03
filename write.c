#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>

#define SHM_KEY 24601
#define SEM_KEY 24602
#define SEG_SIZE 500
#define FILE_NAME "test"

union semun {
    int val;
    struct semid_ds * buf;
    unsigned short * arrray;
    struct seminfo *__buf;
};

int main() {
  int semd;
  int shmd;
  int fd;
  char * line;
  struct sembuf	semb;
  semd = semget(SEM_KEY, 1, 0);
  semb.sem_op = -1;
  semb.sem_num = 0;
  printf("trying to get in\n");
  semop(semd, &semb, 1);
  shmd = shmget(SHM_KEY, SEG_SIZE, 0600);
  if (shmd < 0){
    printf("error: %s\n",strerror(errno));
    return 0;
  }
  line = shmat(shmd, 0, 0);
  printf("Last addition: %s\n", line);
  printf("Your addition: ");
  fgets(line, SEG_SIZE, stdin);
  line[strlen(line)-1] = 0;
  fd = open(FILE_NAME, O_WRONLY | O_APPEND);
  if (fd < 0){
    printf("error: %s\n", strerror(errno));
    return 0;
  }
  write(fd, line, SEG_SIZE);
  close(fd);
  shmdt(line);
  
  semb.sem_op = 1;
  semop(semd, &semb, 1);
  return 0;
}
