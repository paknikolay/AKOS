#include <stdio.h>
#include <stdbool.h>
#include <zconf.h>
#include <memory.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <bits/fcntl-linux.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include <sys/sem.h>
#include <bits/signum.h>
#include <signal.h>

#define SIZE 1024
#define MANAGER_PORT 4444
#define NUM_MAPPERS 1
#define NUM_FILES 2
sem_t* sems[NUM_MAPPERS];
sem_t * sem;
sem_t * sem1;
bool isEnd = false;
int keysReducers[SIZE];
int managerSock;

void handler(){
    signal(SIGINT, handler);

}

void strToInt(int* a, char* s){
    int b = 0;
    int i = 0;
    while(s[i]>='0' && s[i] <= '9'){
        b = b*10 +s[i] - '0';
        ++i;
    }
    *a = b;
}
void intToStr(int a, char *s ) {
    char s1[SIZE];
    bzero(s1, SIZE/2);
    bzero(s, SIZE/2);
    s1[0] = '0';
    int i = 0;
    while (a > 0) {
        s1[i] = '0' + a % 10;
        ++i;
        a=a/10;
    }
    if(i >0) --i;
    int j = 0;
    while (i > 0) {
        s[j] = s1[i];
        ++j;
        --i;
    }
    s[j] = s1[i];

}


int readTwoIntInLine(int fd, int* first, int* second) {
    char tmp;
    char str[SIZE];
    bzero(str, SIZE);
    int i = 0;
    int k = 0;
    if (k = read(fd, &tmp, 1) <= 0)
        return -1;
    while (!((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z') || (tmp >= '0' && tmp <= '9'))) {
        if (read(fd, &tmp, 1) <= 0)return -1;
    }

    while (((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z') || (tmp >= '0' && tmp <= '9'))) {
        str[i] = tmp;
        if (read(fd, &tmp, 1) <= 0)return -1;
        ++i;
    }
    strToInt(first, str);
    bzero(str, SIZE);
    i = 0;
    while (!((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z') || (tmp >= '0' && tmp <= '9'))) {
        if (read(fd, &tmp, 1) <= 0)return -1;
    }

    while (((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z') || (tmp >= '0' && tmp <= '9'))) {
        str[i] = tmp;
        if (read(fd, &tmp, 1) <= 0)return -1;
        ++i;
    }
    strToInt(second, str);
    bzero(str, SIZE);


    return i;
}

int readLine(char* str, int fd) {
    char tmp;
    bzero(str, SIZE);
    int i = 0;
    int k = 0;
    if (k = read(fd, &tmp, 1) <= 0)
        return -1;
    while (tmp != '\n') {
        while (!((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z') || (tmp >= '0' && tmp <= '9'))) {
            if (read(fd, &tmp, 1) <= 0)return -1;
        }

        while (((tmp >= 'a' && tmp <= 'z') || (tmp >= 'A' && tmp <= 'Z') || (tmp >= '0' && tmp <= '9'))) {
            str[i] = tmp;
            if (read(fd, &tmp, 1) <= 0)return -1;
            ++i;
        }
        str[i] = ' ';
        ++i;
    }
    str[i-1] = 0;
    return i;
}

void* mapper(void* args) {
    int id = (int) args;
    int fromManagerToMapper[2];//information
    int toManagerFromMapper[2];//
    if (pipe(fromManagerToMapper) == -1) {
        printf("ERROR: cannot open pipe\n");
        exit(-1);
    };
    if (pipe(toManagerFromMapper) == -1) {
        printf("ERROR: cannot open pipe\n");
        exit(-1);
    };

    int pid = fork();
    if (pid == -1) {
        printf("ERROR:: can't fork");
        exit(-1);
    }
    if (pid == 0) {//child
        dup2(fromManagerToMapper[0], 0);
        close(fromManagerToMapper[0]);
        close(fromManagerToMapper[1]);

        dup2(toManagerFromMapper[1], 1);
        close(toManagerFromMapper[1]);
        close(toManagerFromMapper[0]);
        char dir[SIZE];
        bzero(dir, SIZE);
        dir[0] = '.';
        dir[1] = '/';
        intToStr(id, dir + 2);
        int a = chdir(dir);
        if (execve("../Mapper", NULL, NULL) == -1) {
            fprintf(stderr, "ERROR: cannot find mapper\n");
            exit(-1);
        }

    } else {
        close(fromManagerToMapper[0]);
        close(toManagerFromMapper[1]);

        for (int i = 0; i < NUM_FILES; ++i) {
            char file[SIZE];
            char fileName[SIZE];
            bzero(file, SIZE);
            bzero(fileName, SIZE);
            fileName[0] = '.';
            fileName[1] = '/';
            file[0] = '.';
            file[1] = '/';
            intToStr(id, file + 2);
            file[strlen(file)] = '/';
            intToStr(i, file + strlen(file));
            intToStr(i, fileName + strlen(fileName));
            int len = strlen(file);
            unlink(file);
            int fd = open(file, O_CREAT | O_RDWR | O_SYNC, 77777);
            if (fd == -1) {
                fprintf(stderr, "can not open file %s\n", file);
            }

            int size = abs(rand()) % 200 + 100;

            int u = ftruncate(fd, size);
            if (u == -1) {
                fprintf(stderr, "can not truncate file %s\n", file);
            }
            char *arr;
            arr = (char *) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (arr == NULL) {
                fprintf(stderr, "can not mmap file %s\n", file);
            }

            for (int j = 0; j < size; ++j) {
                arr[j] = 'a' + (j % 25);
                if (rand() % 2 == 0)arr[j] = ' ';
                if (rand() % 5 == 0)arr[j] = '\n';
            }
            munmap(arr, size);

            close(fd);
            if (i != NUM_FILES - 1)fileName[strlen(fileName)] = '\ny\n';
            else fileName[strlen(fileName)] = '\n n \n';
            fprintf(stderr, "(%s)    (%s)\n", file, fileName);
            write(fromManagerToMapper[1], fileName, strlen(fileName));
        }

        int key, value;

        while (readTwoIntInLine(toManagerFromMapper[0], &key, &value)) {
            if (key == -1) {
                break;
            }
            if (keysReducers[key] == -1 && value > 0) {
                sem_wait(sems[id]);
                if (keysReducers[key] == -1) {
                    sem_wait(sem1);
                    char buffer[SIZE];
                    bzero(buffer, SIZE);
                    buffer[0] = 'R';//request
                    write(managerSock, buffer, SIZE);
                    bzero(buffer, SIZE);
                    intToStr(key, buffer);
                    write(managerSock, buffer, SIZE);
                    bzero(buffer, SIZE);
                    sem_post(sem1);
                    read(managerSock, buffer, SIZE);
                    int port;
                    strToInt(&port, buffer);

                    while (true) {
                        int sock;
                        struct sockaddr_in addr;

                        sock = socket(AF_INET, SOCK_STREAM, 0);
                        if (sock < 0) {
                            perror("socket");
                            continue;
                            //exit(1);
                        }

                        addr.sin_family = AF_INET;
                        addr.sin_port = htons(port);
                        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                        if (connect(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
                            perror("connect");
                            exit(2);
                        }
                        keysReducers[key] = sock;
                        break;
                    }
                }
                sem_post(sems[id]);
            }

            char buffer[SIZE];
            intToStr(key, buffer);
            buffer[strlen(buffer)] = ' ';
            intToStr(value, buffer + strlen(buffer));
            buffer[strlen(buffer)] = '\n';

            write(keysReducers[key], buffer, SIZE);

            key = -1;
            value = -1;

        }
        char buffer[SIZE];
        bzero(buffer, SIZE);
        buffer[0] = '\0';
        sem_post(sem);

    }
}

int main() {
    signal(SIGINT, handler);
    for (int j = 0; j < SIZE; ++j) {
        keysReducers[j] = -1;
    }



    char name[2];
    for (int i = 0; i < NUM_MAPPERS; ++i) {
        name[0] = 'e';
        name[1] = '0' + i;
        sem_unlink(name);
        sems[i] = sem_open(name, O_CREAT | O_RDWR, 0777, 0);
        sem_post(sems[i]);
    }


    sem_unlink("sem1");
    sem = sem_open("sem1", O_CREAT | O_RDWR, 0777, 0);

    sem_unlink("sem5");
    sem1 = sem_open("sem5", O_CREAT | O_RDWR, 0777, 1);

    struct sockaddr_in addr;

    managerSock = socket(AF_INET, SOCK_STREAM, 0);
    if (managerSock < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(MANAGER_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(managerSock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("connect");

        exit(2);
    }
    char buffer[SIZE];
    bzero(buffer, SIZE);
    buffer[0] = 'M';
    write(managerSock, buffer, SIZE);

    read(managerSock, buffer, SIZE); //wait for start
    int pid;
    int i = 0;
    pthread_t pthreads[NUM_MAPPERS];
    for (; i < NUM_MAPPERS; ++i) {
        pthread_create(&pthreads[i], 0, &mapper, (void *) i);
    }
    bzero(buffer, SIZE);
    buffer[0] = '\0';

    for (int k = 0; k < NUM_MAPPERS; ++k) {
        sem_wait(sem);//waiting for end mappers
    }

    for (int l = 0; l < SIZE; ++l) {
        if (keysReducers[l] != -1) {
            write(keysReducers[l], buffer, SIZE);
            close(keysReducers[l]);
        }
    }
    bzero(buffer, SIZE);
    buffer[0] = 'E';

    write(managerSock, buffer, SIZE); //wait for end

    return 0;
}