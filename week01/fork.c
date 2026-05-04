#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
	printf("[before fork] pid: %d, ppid: %d\n", getpid(), getppid());

	pid_t pid = fork();
	
	if (pid == -1) {
		perror("fork");
		return 1;
	}

	if (pid == 0){
		printf("[fork child] pid: %d, ppid: %d\n", getpid(), getppid());
		return 0;
	}

	printf("[fork parent] pid: %d, ppid: %d, child_pid: %d\n", getpid(), getppid(), pid);
	
	waitpid(pid, NULL, 0);

	return 0;

}
