/* Пример коммуникации процессов при помощи программных каналов
 * (трубы, pipes).
 *      Данная программа превращается в две программы,
 *      соединенные трубами в таком порядке:
 *
 *         stdout                  stdin
 *        /------------ PIP1 -----------> cmd2
 *      cmd1 <----------PIP2---------------/
 *         stdin                   stdout
 */
/* файл LOOP_strt.c */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define eq(s1,s2) ( strcmp(s1,s2) == 0 ) /* истина, если строки равны */
#define SEP         "---"                /* разделитель команд при наборе */

void main()
{
    char **p, **q;
    int pid;
    int PIP1[2];    /* труба cmd1-->cmd2 */
    int PIP2[2];    /* труба cmd2-->cmd1 */

    pipe(PIP1);   /* создаем две трубы */
    pipe(PIP2);   /* PIP[0] - открыт на чтение, PIP[1] - на запись */

    if (pid != fork()) {      /* развилка: порождаем процесс */
/* ПОРОЖДЕННЫЙ ПРОЦЕСС */
        fprintf(stderr, "сын= pid= %d",  getpid());

/* перенаправляем stdout нового процесса в PIP1 */
        dup2(PIP1[1], 1);
        close(PIP1[1]);
/* канал чтения мы не будем использовать */
        close(PIP1[0]);

/* перенаправляем stdin из PIP2 */
        dup2(PIP2[0], 0);
        close(PIP2[0]);
/* канал записи мы не будем использовать */
        close(PIP2[1]);
        printf("Hello");

/* начинаем выполнять программу, содержащуюся в
 * файле p[0] с аргументами p (т.е. cmd1)
 */
/* возврата из сисвызова exec не бывает */
    } else {
/* ПРОЦЕСС-РОДИТЕЛЬ */
        fprintf(stderr, "отец=%s pid=%d\n", q[0], getpid());

/* перенаправляем stdout в PIP2 */
        dup2(PIP2[1], 1);
        close(PIP2[1]);
        close(PIP2[0]);

/* перенаправляем stdin из PIP1 */
        dup2(PIP1[0], 0);
        close(PIP1[0]);
        close(PIP1[1]);
        char p[100000];
        scanf(p);
        printf(stderr,p);

/* запускаем cmd2 */
    }
}