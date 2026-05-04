#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)

void print_netns(const char *label) {
    char ns_link[256];

    ssize_t len = readlink("/proc/self/ns/net", ns_link, sizeof(ns_link) - 1);
    if (len == -1) {
        perror("readlink");
        exit(EXIT_FAILURE);
    }

    ns_link[len] = '\0';

    printf("%-24s pid=%-8d ppid=%-8d net_ns=%s\n",
           label, getpid(), getppid(), ns_link);
    fflush(stdout);
}

int child_function() {
    print_netns("[child before unshare]");

    if (unshare(CLONE_NEWNET) == -1) {
        perror("unshare");
        exit(EXIT_FAILURE);
    }

    print_netns("[child after unshare]");

    sleep(10000);

    return 0;
}

int main() {
    print_netns("[parent]");

    char *stack = malloc(STACK_SIZE);
    if (stack == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    char *stack_top = stack + STACK_SIZE;

    pid_t clone_pid = clone(child_function, stack_top, SIGCHLD, NULL);
    if (clone_pid == -1) {
        perror("clone");
        free(stack);
        exit(EXIT_FAILURE);
    }

    waitpid(clone_pid, NULL, 0);

    free(stack);

    return 0;
}
