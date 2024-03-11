#include <stdio.h>

#ifdef DYNAMIC_LIB
#include <dlfcn.h>
typedef int (*CollatzConjectureFunc)(int);
typedef int (*TestCollatzConvergenceFunc)(int, int);
#else
#include "collatz.h"
#endif

int main() {

#ifdef DYNAMIC_LIB
    void *libHandle = dlopen("./libcollatz.so", RTLD_LAZY);

    if (!libHandle) {
        fprintf(stderr, "Error: %s\n", dlerror());
        return 1;
    }

    CollatzConjectureFunc collatz_conjecture = (CollatzConjectureFunc)dlsym(libHandle, "collatz_conjecture");
    TestCollatzConvergenceFunc test_collatz_convergence = (TestCollatzConvergenceFunc)dlsym(libHandle, "test_collatz_convergence");

    if (!collatz_conjecture || !test_collatz_convergence) {
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(libHandle);
        return 1;
    }
#endif


    int input_numbers[] = {7, 1, 4, 123, 666};
    int max_iterations = 100;

    int length = sizeof(input_numbers) / sizeof(input_numbers[0]);

    for (int i = 0; i < length; i++) {
        int convergence_result = test_collatz_convergence(input_numbers[i], max_iterations);

        if (convergence_result != -1) {
            printf("Number %d converged to 1 after %d iterations\n", input_numbers[i], convergence_result);
        } else {
            printf("Number %D did not converge to 1 within %d iterations\n", input_numbers[i], max_iterations);
        }
    }

#ifdef DYNAMIC_LIB
    dlclose(libHandle);
#endif

    return 0;
}
