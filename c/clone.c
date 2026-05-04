#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>

#define STACK_SIZE 1024*1024

void print_stat(const char *label) {
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

int child_function(void *arg){
	print_stat("[child]");
	
	sleep(10000);

	return 0;
}

int main(){
	
	print_stat("[parent]");

	char *stack = malloc(STACK_SIZE);
	if (!stack) {
		perror("malloc");
		return 1;
	}

	char *stack_top = stack + STACK_SIZE;

	pid_t clone_pid = clone(child_function, stack_top, CLONE_NEWNET | SIGCHLD, NULL);
	
	if (clone_pid == -1) {
		perror("clone");
		free(stack);
		return 1;
	}

	printf("Parent process continues after clone().\n");

	waitpid(clone_pid, NULL, 0);

	free(stack);

	return 0;

}
