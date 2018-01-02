#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <strings.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <fcntl.h>
#include <signal.h>

#define PORT 4444
#define  BUFFERSIZE 1024

#define NUM_MAP_MANAGER 4
#define NUM_REDUCER_MANAGER 3
#define NUM_CONNECTIONS (NUM_MAP_MANAGER + NUM_REDUCER_MANAGER)
#define SIZE 1024
int socks[NUM_CONNECTIONS];
pthread_t pthreads[SIZE];
char name[NUM_CONNECTIONS];
pthread_mutex_t mutex;
int managerReducers[NUM_REDUCER_MANAGER];
int managerMappers[NUM_MAP_MANAGER];
int reducersPorts[SIZE];
int storage[SIZE];
int reducersKeyNumber =0;
int REDUCERS_AMOUNT;
int reducersCount = 0;

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
sem_t* sem;
sem_t* sem1;
bool isEnd = false;
sem_t* semsMap[NUM_CONNECTIONS];
sem_t* semsRed[NUM_CONNECTIONS];
void* mapperManager(void* arg) {
    int ind = (int) arg;
    sem_wait(semsMap[ind]);
    int fd = managerMappers[ind];
    write(fd,"S", SIZE);
    char buffer[SIZE];
    while (true) {
        bzero(buffer, SIZE);
        read(fd, buffer, SIZE);
        /*if(buffer[0] == 0){
            fprintf(stderr,"ERROR wrong data\n");
            exit(-1);
        }*/
        if (buffer[0] == 'R') {//request
            bzero(buffer,SIZE);
            read(fd, buffer, SIZE);
            int key;
            strToInt(&key, buffer);
            if (reducersPorts[key] == -1) {
                ++reducersKeyNumber;
                char buffer[SIZE];
                bzero(buffer, SIZE);
                reducersPorts[key] = reducersPorts[reducersKeyNumber % REDUCERS_AMOUNT];
                //назначить key менеджеру  редьюсеру
            }
            char buffer[SIZE];
            bzero(buffer, SIZE);
            intToStr(reducersPorts[key], buffer);
            write(fd, buffer, SIZE);

        } else {   //end
            sem_post(sem);
            break;
        }
    }
}

void* reducerManager(void* arg) {
    int ind = (int) arg;

    int fd = managerReducers[ind];

    char buffer[SIZE];
    sem_wait(semsRed[ind]);
    write(fd,"S", SIZE);

    for(int i = 0; i < REDUCERS_AMOUNT; ++i) {

        char port[SIZE];
        bzero(port, SIZE);
        read(fd, port, SIZE);
        int port1;
        strToInt(&port1, port);

        sem_wait(sem1);
        reducersPorts[reducersCount] = port1;
        ++reducersCount;
        sem_post(sem1);
    }
    sem_post(sem);

    while (true) {
        read(fd, buffer, SIZE);
        if (buffer[0] == 'A') {//answers
            bzero(buffer,SIZE);
            read(fd, buffer, SIZE);
            int key;
            strToInt(&key, buffer);
            bzero(buffer,SIZE);
            read(fd, buffer, SIZE);
            int value;
            strToInt(&value, buffer);
            sem_wait(sem1);
            storage[key]+= value;
            sem_post(sem1);
        } else {  //end
            sem_post(sem);
            break;
        }
    }
}


int main() {
    signal(SIGINT, handler);

    char name[2];
    for (int i = 0; i < NUM_CONNECTIONS; ++i) {
        name[0] = 'q';
        name[1] = '0' + i;
        sem_unlink(name);
        semsMap[i] = sem_open(name, O_CREAT | O_RDWR, 0777, 0);
    }

    for (int i = 0; i < NUM_CONNECTIONS; ++i) {
        name[0] = 'w';
        name[1] = '0' + i;
        sem_unlink(name);
        semsRed[i] = sem_open(name, O_CREAT | O_RDWR, 0777, 0);
    }


    sem_unlink("sem4");
    sem1 = sem_open("sem4", O_CREAT | O_RDWR, 0777, 1);


    sem_unlink("sem3");
    sem = sem_open("sem3", O_CREAT | O_RDWR, 0777, 0);


    for (int i = 0; i < SIZE; ++i) {
        reducersPorts[i] = -1;
        storage[i] = 0;
    }

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
    int amount_of_mapper_manager = 0;
    int amount_of_reducer_manager = 0;
    while (amount_of_mapper_manager < NUM_MAP_MANAGER || amount_of_reducer_manager < NUM_REDUCER_MANAGER) {
        sock = accept(listener, NULL, NULL);
        socks[amount_of_connections] = sock;
        if (sock < 0) {
            perror("accept");
            exit(3);
        }

        char buffer[SIZE];
        read(sock, buffer, SIZE);
        if (buffer[0] == 'M') {//mapperManager
            managerMappers[amount_of_mapper_manager] = sock;
            pthread_create(&pthreads[amount_of_connections], 0, &mapperManager, (void *) amount_of_mapper_manager);
            ++amount_of_connections;
            ++amount_of_mapper_manager;
        } else {//reducerManager
            bzero(buffer, SIZE);
            read(sock, buffer, SIZE);
            strToInt(&REDUCERS_AMOUNT, buffer);

            managerReducers[amount_of_reducer_manager] = sock;
            pthread_create(&pthreads[amount_of_connections], 0, &reducerManager, (void *) amount_of_reducer_manager);
            ++amount_of_connections;
            ++amount_of_reducer_manager;
        }
    }

    for (int l = 0; l < NUM_REDUCER_MANAGER; ++l) {
        sem_post(semsRed[l]);
    }


    for (int j = 0; j < NUM_REDUCER_MANAGER; ++j) {
        sem_wait(sem);
    }

    REDUCERS_AMOUNT *= NUM_REDUCER_MANAGER;

    for (int l = 0; l < NUM_MAP_MANAGER; ++l) {
        sem_post(semsMap[l]);
    }

    for (int j = 0; j < NUM_MAP_MANAGER; ++j) {
        sem_wait(sem);
    }

    char buffer[SIZE];
    buffer[0] = 'E';
    for (int i = 0; i < NUM_REDUCER_MANAGER; ++i) {
        write(managerReducers[i], buffer, SIZE);
    }

    for (int j = 0; j < NUM_REDUCER_MANAGER; ++j) {
        sem_wait(sem);
    }
    sleep(3);
    for (int k = 0; k < SIZE; ++k) {
        if (storage[k] > 0) {
            printf("%c %d\n", ('a' + k), storage[k]);
        }
    }

    return 0;
}