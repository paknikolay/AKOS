#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>

int main() {
    void *handle;
    int (*amount2)(int, ...);
    char *error;

    handle = dlopen("./lib.so", RTLD_LAZY);
    if (!handle) {
        fputs(dlerror(), stderr);
        exit(1);
    }

    amount2 = dlsym(handle, "amount2");
    if ((error = dlerror()) != NULL) {
        fputs(error, stderr);
        exit(1);
    }

    printf("%d\n", amount2(9, 2, 4, 5 ,8 ,6,4, 7, 6,7));
    dlclose(handle);
    return 0;
}