#include <stdio.h>
#include <stdbool.h>
#include <zconf.h>
#include <memory.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <bits/signum.h>
#include <signal.h>

#define NUM_REDUCERS 1
#define SIZE 1024
#define MANAGER_PORT 4444
int managerSock;
struct Reducer{
    int socketFd;
    int toReducerFd;

};
struct Reducer reducers[NUM_REDUCERS][SIZE];
sem_t* sems[NUM_REDUCERS];
int PORTS[SIZE];
int *storage;
sem_t* sem, *sem2;
bool isEnd = false;
int isReady = 0;
int DEFAULT_PORT;


void handlerPipe(){
    signal(SIGPIPE, handlerPipe);
}

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

void * reducerHelper(void* args){

    int a = (int) args;
    char *tmp = &a;
    int id = *tmp;

    int num =  *(tmp + 1);

    int socketFd = reducers[id][num].socketFd;
    int toReducerFd= reducers[id][num].toReducerFd;

    int key, value;

    while(readTwoIntInLine(socketFd,&key, &value)){
        if(key == -1){
            write(toReducerFd, "\0", 1);
            break;
        }

        char keyValue[SIZE];
        intToStr(key, keyValue);
        keyValue[strlen(keyValue)] = ' ';
        intToStr(value, keyValue + strlen(keyValue));
        keyValue[strlen(keyValue)]='\n';

        sem_wait(sems[id]);
        write(toReducerFd, keyValue, strlen(keyValue));
        sem_post(sems[id]);

        key = -1;
        value = -1;
    }

}


void* reducer(void* args) {

    int id = (int) args;
    int PORT = DEFAULT_PORT + id;
    char port[SIZE];
    intToStr(PORT, port);
    port[strlen(port)] = '\n';
    sem_wait(sem2);
    write(managerSock, port, SIZE);
    sem_post(sem2);

    pthread_t pthreads[SIZE];

    int fromManagerToReducer[2];//information
    int toManagerFromReducer[2];//
    if (pipe(fromManagerToReducer) == -1) {
        printf("ERROR: cannot open pipe\n");
        exit(-1);
    };
    if (pipe(toManagerFromReducer) == -1) {
        printf("ERROR: cannot open pipe\n");
        exit(-1);
    };

    int pid = fork();
    if (pid == -1) {
        printf("ERROR:: can't fork");
        exit(-1);
    }
    if (pid == 0) {//child
        dup2(fromManagerToReducer[0], 0);
        close(fromManagerToReducer[0]);
        close(fromManagerToReducer[1]);

        dup2(toManagerFromReducer[1], 1);
        close(toManagerFromReducer[1]);
        close(toManagerFromReducer[0]);

        if (execve("Reducer", NULL, NULL) == -1) {
            fprintf(stderr, "ERROR: cannot find Reducer\n");
            exit(-1);
        }

    } else {
        close(fromManagerToReducer[0]);
        close(toManagerFromReducer[1]);

        int pid2 = fork();
        if (pid2 == -1) {
            fprintf(stderr, "ERROR:: reducer:: can't fork");
            exit(-1);
        }
        if (pid2 != 0) {
            close(toManagerFromReducer[0]);


            int sock, listener;
            struct sockaddr_in addr;
            char buf[1024];
            int bytes_read;

            listener = socket(AF_INET, SOCK_STREAM, 0);
            if (listener < 0) {
                perror("socket");
                exit(1);
            }

            addr.sin_family = AF_INET;
            addr.sin_port = htons(PORT);
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            if (bind(listener, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
                perror("bind");
                exit(2);
            }

            listen(listener, 1);

            int amount_of_connections = 0;
            while (true) {
                sock = accept(listener, NULL, NULL);
                reducers[id][amount_of_connections].socketFd = sock;
                reducers[id][amount_of_connections].toReducerFd = fromManagerToReducer[1];
                if (sock < 0) {
                    perror("accept");
                    exit(3);
                }
                int a;
                char *c = (char *) (&a);
                c[0] = id;
                c[1] = amount_of_connections;
                pthread_create(&pthreads[amount_of_connections], 0, &reducerHelper, (void *) a);
                ++amount_of_connections;
            }
        } else {
            char buffer[SIZE];
            int key = 0, amount = 0;
            int last_key = 0;
            while (readTwoIntInLine(toManagerFromReducer[0], &key, &amount) > 0) {
                if (key < last_key)break;
                storage[key] += amount;
                last_key = key;
            }


            write(fromManagerToReducer[1], "\0", 1);
            sem_post(sem);
        }
    }
}

int main() {
    signal(SIGINT, handler);
    signal(SIGPIPE, handlerPipe);
    srand(time(NULL));
    DEFAULT_PORT = abs((rand() * 1000 + rand())) % 500 + 4444;

    int s = shm_open("storage", O_RDWR | O_CREAT, 0777);
    int u = ftruncate(s, SIZE);
    storage = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, s, 0);

    for (int i = 0; i < SIZE; ++i) {
        storage[i] = 0;
    }
    char name[2];
    int add = rand() % 20;
    for (int i = 0; i < NUM_REDUCERS; ++i) {
        name[0] = 'w';
        name[1] = '0' + i + add;
        sem_unlink(name);
        sems[i] = sem_open(name, O_CREAT | O_RDWR, 0777, 0);
        sem_post(sems[i]);
    }

    char b[] = "sem";
    b[2] += add;
    sem_unlink("b");
    sem = sem_open("b", O_CREAT | O_RDWR, 0777, 0);

    char c[] = "sem2";
    c[2] += add;
    sem_unlink("c");
    sem2 = sem_open("c", O_CREAT | O_RDWR, 0777, 1);

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

    char *buffer[SIZE];
    bzero(buffer, SIZE);
    buffer[0] = 'R';
    write(managerSock, buffer, SIZE);
    bzero(buffer, SIZE);
    intToStr(NUM_REDUCERS, buffer);
    write(managerSock, buffer, SIZE);

    read(managerSock, buffer, SIZE);//start
    int pid;
    int i = 0;
    pthread_t pthreads[NUM_REDUCERS];
    for (; i < NUM_REDUCERS; ++i) {
        pthread_create(&pthreads[i], 0, &reducer, (void *) i);
    }

    read(managerSock, buffer, SIZE);//end
    isEnd = true;

    for (int i = 0; i < NUM_REDUCERS; ++i) {
        sem_wait(sem);
    }


    for (int i = 0; i < SIZE; ++i) {

        if (storage[i] != 0) {

            bzero(buffer, SIZE);
            buffer[0] = 'A';
            write(managerSock, buffer, SIZE);
            sem_wait(sem2);
            char buffer[SIZE];
            intToStr(i, buffer);
            buffer[strlen(buffer)] = ' ';
            write(managerSock, buffer, SIZE);
            bzero(buffer, SIZE);
            bzero(buffer, SIZE);
            intToStr(storage[i], buffer);
            buffer[strlen(buffer)] = '\n';
            write(managerSock, buffer, SIZE);
            sem_post(sem2);
        }
    }
    bzero(buffer, SIZE);
    buffer[0] = 'E';
    write(managerSock, buffer, SIZE);
    close(managerSock);
    return 0;
}