#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MSGSIZE 200

int main(void) {

	char inbuf[MSGSIZE];
	pid_t childpid, childpid2;
	int fd[2];

	if ((pipe(fd) == -1) || ((childpid = fork()) == -1)) {
		perror("Failed to setup pipeline");
		return 1;
	}

	if (childpid == 0) { 				//Child
		//printf("Inside the first child\n");
		if (dup2(fd[1], STDOUT_FILENO) == -1){
			perror("Failed to redirect stdout of tail");
		}
		
		else if ((close(fd[0]) == -1) || (close(fd[1]) == -1)){
			perror("Failed to close extra pipe descriptors on tail");
		}	
		
		else {
			//printf("About to run tail\n");
			execl("/usr/bin/tail", "tail", "-5", "b.txt", NULL);
			perror("Failed to exec tail");
		}
		return 1;
		
	}
	else{								//Parent

		wait(NULL);
		childpid2 = fork();

		if(childpid2 == 0){
			printf("In child2\n");
			if (dup2(fd[1], STDOUT_FILENO) == -1){
				perror("Failed to redirect stdout of grep");
			}
		
			else if ((close(fd[0]) == -1) || (close(fd[1]) == -1)){
				perror("Failed to close extra pipe descriptors on grep");
			}	
			
			else {
				execl("/usr/bin/grep", "grep", "ee", NULL);
				perror("Failed to exec grep");
			}
			return 1;
		}

		else{

			wait(NULL);
			printf("In parent\n");

			if (dup2(fd[0], STDIN_FILENO) == -1){ /* sort is the parent */
				perror("Failed to redirect stdin of sort");
			}
			else if ((close(fd[0]) == -1) || (close(fd[1]) == -1)){
				perror("Failed to close extra pipe file descriptors on sort");
			}
			else {
				read(fd[0], inbuf, MSGSIZE);
				printf("%s\n", inbuf);
				//execl("/usr/bin/sort", "sort", NULL);
				//perror("Failed to exec sort");
			}
			return 1;
		}
	}
}