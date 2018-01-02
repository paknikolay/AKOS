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
//SELECT
#define PORT 5555
#define  BUFFERSIZE 1024
#define NUM_CONNECTIONS 2
int socks[NUM_CONNECTIONS];
pthread_t pthreads[NUM_CONNECTIONS];
char field[BUFFERSIZE];
int n = 20, m = 20;
int isAlive = 1;

char name[NUM_CONNECTIONS];
sem_t * semWorker[NUM_CONNECTIONS];
sem_t * semMaster [NUM_CONNECTIONS];

pthread_mutex_t mutex;



void* life(void* args) {

    int ind = (int) args;

    int sock = socks[ind];
    char buf[BUFFERSIZE];
    bzero(buf, BUFFERSIZE);

    long long local_step = 0;
    int f1, f2, l1, l2;


    f1 = ind * (n / NUM_CONNECTIONS);
    l1 = (ind + 1) * (n / NUM_CONNECTIONS);
    if (l1 > n || ind == NUM_CONNECTIONS - 1)
        l1 = n;
    l2 = m;
    f2 = 0;

    pthread_mutex_lock(&mutex);
    printf("%d, %d, %d\n", ind, f1, l1);
    pthread_mutex_unlock(&mutex);

    buf[0] = n;
    int size = buf[0];
    buf[1] = l1 - f1 + 2;
    int height = buf[1];

    pthread_mutex_lock(&mutex);
    printf("send data: bytes %d, size: %d, height: %d  ind: %d\n", write(sock, buf, BUFFERSIZE), size, height, ind);
    pthread_mutex_unlock(&mutex);

    bzero(buf, BUFFERSIZE);


    char localField[BUFFERSIZE];
    bzero(localField, BUFFERSIZE);

    while(isAlive) {
        pthread_mutex_lock(&mutex);
        printf("waiting for master %d) \n", ind);
        pthread_mutex_unlock(&mutex);

        sem_wait(semWorker[ind]);

        //sem_wait(semWorker[ind]);
        //sem_wait(semWorker[ind]);

        pthread_mutex_lock(&mutex);
        printf("start working %d\n", ind);
        pthread_mutex_unlock(&mutex);


        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < n; ++j) {
                localField[i * n + j] = field[(f1 +i - 1 + n) % n * n + j];
         //       printf("%d ", localField[i * n + j]);
            }
           // printf("\n");
        }


        pthread_mutex_lock(&mutex);
        printf("send data: bytes %d, ind: %d\n", write(sock, localField, BUFFERSIZE), ind);
        pthread_mutex_unlock(&mutex);

        bzero(localField, BUFFERSIZE);

        pthread_mutex_lock(&mutex);
        printf("got field %d, ind: %d\n", read(sock, localField, BUFFERSIZE), ind);
        pthread_mutex_unlock(&mutex);

        sem_post(semMaster[ind]);
        sem_wait(semWorker[ind]);

        for (int i = 1; i < height-1; ++i) {
            for (int j = 0; j < n; ++j) {
                field[(f1 +i - 1 + n) % n * n + j] = localField[i * n + j] ;
           //     printf("%d ", localField[i * n + j]);
            }
         //   printf("\n");
        }
        sem_post(semMaster[ind]);
    }

}

void master(){

    pthread_mutex_lock(&mutex);
    printf("Start working FIELD:\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%d ", field[i * n + j]);
        }
        printf("\n");
    }
    pthread_mutex_unlock(&mutex);


    for(int i = 0; i< NUM_CONNECTIONS; ++i){
        sem_post(semWorker[i]);
    }

    pthread_mutex_lock(&mutex);
    pthread_mutex_unlock(&mutex);



    while(isAlive){
        for(int i = 0; i< NUM_CONNECTIONS; ++i){
            sem_wait(semMaster[i]);
        }
       // bzero(field, BUFFERSIZE);
        for(int i = 0; i< NUM_CONNECTIONS; ++i){
            sem_post(semWorker[i]);
        }

        for(int i = 0; i< NUM_CONNECTIONS; ++i){
            sem_wait(semMaster[i]);
        }


        sleep(1);

        pthread_mutex_lock(&mutex);

        printf("\n master field::\n");
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (field[i*n + j] == 0)
                    printf(". ");
                else
                    printf("I ");
            }
            printf("\n");
        }

        printf("\n");


        pthread_mutex_unlock(&mutex);


        printf("\n master field end::\n");
        for(int i = 0; i< NUM_CONNECTIONS; ++i){
            sem_post(semWorker[i]);
        }

    }


}


int main() {
    int sock, listener;
    struct sockaddr_in addr;
    char buf[1024];
    int bytes_read;

    isAlive = 1;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            field[i * n + j] = 0;
        }
    }

    //улий

    /*  field[6 * n + 6] = 1;
      field[6 * n + 7] = 1;
      field[6 * n + 8] = 1;
      field[5 * n + 6] = 1;
  */
//квадратик
    /*
    field[0] = 1;
    field[n] = 1;
    field[n + 1] = 1;
*/
// мигалка
/*
    field[3*n] = 1;
    field[3*n+1] = 1;
    field[3*n+2] = 1;
*/

    //глайдер
    /*     field[3*n] = 1;
         field[3*n +1] = 1;
         field[3*n + 2] = 1;
         field[2*n +2 ] = 1;
         field[n + 1] = 1;
  */
    //навигационные огни
    field[7 *n + 5] = 1;
    field[7 * n + 6] = 1;
    field[7 * n + 7] = 1;
    field[6*n + 6] = 1;
    field[7 * n +5] = 1;
    field[7 * n + 7] = 1;
    field[6 * n + 6] = 1;






    for (int i = 0; i < NUM_CONNECTIONS; ++i) {
        name[0] = 'w';
        name[1] = '0' + i;
        sem_unlink(name);
        semMaster[i] = sem_open(name, O_CREAT | O_RDWR, 0777, 0);
        name[0] = 'e';
        sem_unlink(name);
        semWorker[i] = sem_open(name, O_CREAT | O_RDWR, 0777, 0);
    }


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
    while (amount_of_connections < NUM_CONNECTIONS) {
        sock = accept(listener, NULL, NULL);
        printf("%d\n", sock);
        socks[amount_of_connections] = sock;
        if (sock < 0) {
            perror("accept");
            exit(3);
        }

        pthread_create(&pthreads[amount_of_connections], 0, &life, (void *) amount_of_connections);
        ++amount_of_connections;
    }

    pthread_mutex_lock(&mutex);
    printf("((((((%d)))))) \n", amount_of_connections);
    pthread_mutex_unlock(&mutex);

    master();

    return 0;
}