#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define amount 10

sem_t * semWorker[amount];
sem_t * semMaster [amount];
sem_t *mu;
char name[2];
int n =amount, m = amount;
int size;
int isAlive;
int *fieldNext, *field;


void live(int ind) {
    long long local_step = 0;
    int f1, f2, l1, l2;
    f1 = ind * (n / amount);
    l1 = (ind + 1) * (n / amount);
    if (l1 > n) l1 = n;
    l2 = m;
    f2 = 0;
    while (isAlive) {
        sem_post(semMaster[ind]);
        sem_wait(semWorker[ind]);

        sem_wait(mu);
        printf("(%d, %d\n", local_step/2, ind);
        sem_post(mu);


        for (int i = f1; i < l1; ++i) {
            for (int j = f2; j < l2; ++j) {
                int amountOfAlive = 0;
                for (int q = -1; q <= 1; ++q)
                    for (int w = -1; w <= 1; ++w) {
                        if (q != 0 || w != 0) {
                            //printf("%d(%d %d) ",field[(i + q + n) % n][(j + w + m) % m][0], (i + q + n) % n , (j + w + m) % m);
                            if (field[(i + q + n) % n * n + (j + w + m) % m] == 1)
                                ++amountOfAlive;
                        }
                    }
                /*  pthread_mutex_lock(&mut);
                  if(field[i][j][0]==1) printf("%d %d", i,j);
                  printf("\n%d %d \n________\n",field[i][j][0], amountOfAlive);
                  pthread_mutex_unlock(&mut);
                */
                if (field[i * n + j] == 1) {
                    if (amountOfAlive < 2 || amountOfAlive > 3)
                        fieldNext[i * n + j] = 0;
                    else
                        fieldNext[i * n + j] = 1;
                } else {
                    if (amountOfAlive == 3)
                        fieldNext[i * n + j] = 1;
                    else
                        fieldNext[i * n + j] = 0;
                }
            }
        }

        ++local_step;
        sem_post(semMaster[ind]);
        sem_wait(semWorker[ind]);


        int lifeCount = 0;
        for (int i = f1; i < l1; ++i) {
            for (int j = f2; j < l2; ++j) {
                field[i * n + j] = fieldNext[i * n + j];
                if (field[i * n + j] == 1)
                    ++lifeCount;
            }
        }

        ++local_step;
        sem_wait(mu);
        isAlive += lifeCount;
        sem_post(mu);
        sem_post(semMaster[ind]);
        sem_wait(semWorker[ind]);
    }
    return;
}


void mainFunction() {

    while (isAlive) {
        for(int i = 0; i < amount; ++i)
            sem_wait(semMaster[i]);
        int isAlive = 0;
        int current_step;

        for (int i = 0; i < amount; ++i)
            sem_post(semWorker[i]);
        ;

        for(int i = 0; i < amount; ++i)
            sem_wait(semMaster[i]);
        ++current_step;
        for (int i = 0; i < amount; ++i)
            sem_post(semWorker[i]);

        printf("\n\n%d)\n\n\n", (current_step - 1) / 2);

        for(int i = 0; i < amount; ++i)
            sem_wait(semMaster[i]);

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
        sleep(1);
        ++current_step;
        int * tmp = field;
        field = fieldNext;
        fieldNext = tmp;
        //isAlive = amountOfAlive;
        printf("\n\n");
        for (int i = 0; i < amount; ++i)
            sem_post(semWorker[i]);
    }
    return;
}


int main() {
    size = n * m * sizeof(int) * 50;
    int fInd1 = shm_open("field", O_RDWR | O_CREAT, 0777);
    int u = ftruncate(fInd1, size);

    int fInd2 = shm_open("fieldNext", O_RDWR | O_CREAT, 0777);
    int u2 = ftruncate(fInd2, size);

    field = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fInd1, 0);
    fieldNext = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fInd2, 0);

    int aInd = shm_open("isAlive", O_RDWR | O_CREAT, 0777);
    int u3 = ftruncate(aInd, sizeof(int));

    isAlive = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, aInd, 0);
    isAlive = 1;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            field[i * n + j] = 0;
        }
    }
//улий
/*
    field[6 * n + 6] = 1;
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

    field[3*n] = 1;
    field[3*n+1] = 1;
    field[3*n+2] = 1;


    //глайдер
/*       field[3*n] = 1;
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



    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            printf("%d ", field[i * n + j]);
        }
        printf("\n");
    }



    for (int i = 0; i < n; ++i) {
        name[0] = 'w';
        name[1] = '0' + i;
        // printf("%s \n",name);
        semMaster[i] = sem_open(name, O_CREAT | O_RDWR, 0777, 0);
        ///sem_close(sems[i]);

        /*    int in = shm_open(name, O_RDWR | O_CREAT, 0777);
            int un = ftruncate(in, sizeof(sem_t));
            sem[i] = mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, in, 0);
            sem_init(sem[i], 1 , 0);
    */
        name[0] = 'e';

        /*int in1 = shm_open(name, O_RDWR | O_CREAT, 0777);
        int un1 = ftruncate(in1, sizeof(sem_t));
        sems[i] = mmap(0, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, in1, 0);
        sem_init(sems[i], 1 , 0);
        //printf("%s \n",name);
        */
        semWorker[i] = sem_open(name, O_CREAT | O_RDWR, 0777, 0);
        // sem_close(sem[i]);
    }



    mu = sem_open("m", O_CREAT | O_RDWR, 0777, 1);

    int pId;
    int number;
    for (int i = 0; i < n; ++i) {
        number = i;
        pId = fork();
        if (pId == 0) {
            break;
        }
    }
/*
if (pId > 0){
    for (int i = 0; i < amount ; ++i) {
        sem_wait(sems[number]);
    }
}
    else{
    printf("%d\n", number);
    sem_post(sems[number]);
}*/
    /*  if(pId > 0 || number == n-1){
          if(number != n-1)
              mainFunction();
           else live(9);
       }
      */
    if (pId > 0){
        mainFunction();
    }
    else {
        live(number);
    }

    return 0;
};
