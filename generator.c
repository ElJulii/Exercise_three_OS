#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        return 1;
    }

    int N = atoi(argv[1]);

    if (N < 120 || N > 180) {
        fprintf(stderr, "Error: N must be between 120 and 180.\n");
        return 1;
    }

    srand(time(NULL));

    const char operators[] = {'+', '-', '*', '/'};

    for (int i = 0; i < N; i++) {
        int X = rand() % 9 + 1;
        char O = operators[rand() % 4]; 
        int Y = rand() % 9 + 1; 

        printf("%d %c %d\n", X, O, Y);
        fflush(stdout); 

        sleep(1);
    }

    return 0;
}