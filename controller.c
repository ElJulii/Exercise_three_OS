#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t lines_processed = 0;

void handle_sigusr1(int sig) {
    fprintf(stderr, "Processed %d lines\n", lines_processed);
}

int main() {
    int pipeA[2], pipeB[2], pipeC[2];

    if (pipe(pipeA) == -1 || pipe(pipeB) == -1 || pipe(pipeC) == -1) {
        perror("pipe");
        exit(1);
    }

    struct sigaction sa;
    sa.sa_handler = handle_sigusr1;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGUSR1, &sa, NULL);

    pid_t pid1 = fork();

    if (pid1 == 0) {
        dup2(pipeA[1], STDOUT_FILENO);
        close(pipeA[0]);
        close(pipeA[1]);
        close(pipeB[0]);
        close(pipeB[1]);
        close(pipeC[0]);
        close(pipeC[1]);

        char n_str[4];
        int N = 120 + rand() % 61;
        snprintf(n_str, sizeof(n_str), "%d", N);

        execl("./generator", "./generator", n_str, (char *)NULL);
        perror("execl");
        exit(1);
    }

    pid_t pid2 = fork();

    if (pid2 == 0) { 
        dup2(pipeB[0], STDIN_FILENO);
        dup2(pipeC[1], STDOUT_FILENO);

        close(pipeA[0]);
        close(pipeA[1]);
        close(pipeB[0]);
        close(pipeB[1]);
        close(pipeC[0]);
        close(pipeC[1]);

        execl("/usr/bin/bc", "/usr/bin/bc", (char *)NULL);
        perror("execl");
        exit(1);
    }

    close(pipeA[1]);
    close(pipeB[0]);
    close(pipeC[1]);

    char expression[256];
    char result[256];

    while (1) {
        ssize_t bytes_read = read(pipeA[0], expression, sizeof(expression) - 1);
        if (bytes_read <= 0) {
            break;
        }

        expression[bytes_read] = '\0';

        write(pipeB[1], expression, strlen(expression));

        bytes_read = read(pipeC[0], result, sizeof(result) - 1);
        if (bytes_read <= 0) {
            break;
        }

        result[bytes_read] = '\0';

        printf("%s = %s", expression, result);
        fflush(stdout);

        lines_processed++;
    }

    close(pipeA[0]);
    close(pipeB[1]);
    close(pipeC[0]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
