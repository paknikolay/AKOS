#include <stdio.h>
#include <stdbool.h>
#define SIZE 1024
int main() {
    int key, amount;
    int storage[SIZE];
    for (int i = 0; i < SIZE; ++i) {
        storage[i] = 0;
    }
    int n = 0;
    while (scanf("%d %d", &key, &amount)) {
        storage[key] += amount;
    }

    for (int i = 0; i < SIZE; ++i) {
        if (storage[i] != 0)
            printf("%d %d\n", i, storage[i]);
        storage[i] = 0;
    }

    return 0;
}