#include <stdio.h>
#include <stdbool.h>
#include <zconf.h>
#include <memory.h>
#include <stdlib.h>

#define SIZE 1024

void intToStr(int a, char *s ) {
    char s1[SIZE];
    bzero(s1, SIZE);
    bzero(s, SIZE);
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

int main() {
    char buffer[SIZE];
    while (true) {
        char fileName[SIZE];
        bzero(fileName, SIZE);
        scanf("%s",fileName);
        FILE *file = fopen(fileName, "r+t");
        if(file == NULL){
            fprintf(stderr,"ERROR: cannot open file %s\n", fileName);
            exit(-1);
        }

        char *result;
        char buffer[SIZE];
        while (true) {
            bzero(buffer, SIZE);
            result = fgets(buffer, SIZE, file);
            if (result) {
                int i = 0;
                int alph[33];
                for (int j = 0; j < 33; ++j) {
                    alph[j] = 0;
                }
                while (buffer[i] != '\n') {
                    while (i < SIZE && !((buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= 'A' && buffer[i] <= 'Z'))) {
                        ++i;
                    }

                    if (i < SIZE) {
                        if (buffer[i] - 'a' >= 0)
                            alph[buffer[i] - 'a'] += 1;
                        else
                            alph[buffer[i] - 'A'] += 1;
                        while (i < SIZE && ((buffer[i] =='-')||(buffer[i] == '_')||(buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= 'A' && buffer[i] <= 'Z'))){
                            ++i;
                        }
                    } else
                        break;
                }
                for(char j = 'a'; j <='z';++j){
                   if(alph[j - 'a'] != 0) {
                       int key1 = j - 'a';
                       char key[SIZE];
                       intToStr(key1, key);
                       printf("%s %d\n", key, alph[j - 'a']);
                   }
                }
                fflush(stdout);

            } else
                break;
        }
        fclose(file);
        printf("EOF, continue?\n");
        char answer[SIZE];
        bzero(answer, SIZE);
        scanf("%s",answer);
        if(answer[0] != 'y'){
            printf("End of program\n");
            close(1);
            close(0);
            exit(0);
        }
    }


    return 0;
}