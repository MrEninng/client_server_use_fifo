#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

#include "address.h"

#define		R  0	// Read/stdin
#define		W  1 	// Write/stdout

void run_pr1(int p[], int q[]);

void write_into_p(int p[], const char * buff);

void reader(int q[], int fd);

int main(int argc, char **argv)
{
	puts("I'm WRITER");
	int fd;
	char myFifo2[BUFF_SIZE];
	
	char buff[BUFF_SIZE];	
	int pswdFlag = 1;
	
	while (pswdFlag) {
		puts("Input password");
		fgets(buff, BUFF_SIZE, stdin);
		int fd2 = open(PUB_FIFO, O_WRONLY);
		write(fd2, buff, BUFF_SIZE);
		close(fd2);
		fd2 = open(PUB_FIFO, O_RDONLY);
		int res = read(fd2, buff, BUFF_SIZE);
			if (res < 0) {
				perror("Closed fifo2");
				exit(errno);
			}	
		char *ret = "1\0";
		res = strcmp(buff, ret);
		if (res == 0) {
			printf("Not correct password. Try again.\n");
		} else {
			printf("Password correct. Make input.\n");
			int fd = open(PUB_FIFO, O_RDONLY);
	//		char b[BUFF_SIZE];
			int res = read(fd, myFifo2, BUFF_SIZE);
//			printf("res %d b = %s", res, myFifo2); 
			close(fd);
			pswdFlag = 0;
		}
	}
	while (fgets(buff, BUFF_SIZE, stdin) != NULL) {
		fd = open(myFifo2, O_WRONLY);
		int p[2], q[2];
		pipe(p);
		pipe(q);
		write_into_p(p, buff);
		close(p[W]);
		run_pr1(p, q);
		close(p[R]);
		reader(q, fd);
		close(q[R]);
		close(q[W]);
		int status;
		pid_t pid;
		while ((pid = wait(&status)) != -1); 
		close(fd);
		
		char new_buff[BUFF_SIZE];
		int fifo_fd = open(myFifo2, O_RDONLY);
		int res = read(fifo_fd, new_buff, BUFF_SIZE);
		if (res < 0) {
			perror("Closed fifo2");
			exit(errno);
		}
		close(fifo_fd);
		printf("%s", new_buff);
				
	} 
	//write(fd, buff, sizeof(buff));
	unlink(PUB_FIFO);
	unlink(myFifo2);
	return 0;
}

void run_pr1(int p[], int q[]) 
{
	pid_t res;
	switch (res = fork()) {
		case 0: {
			// Подменяем стдин
			close(p[W]);
			close(R);
			dup(p[R]);
			close(p[R]);
			
			close(q[R]);
			close(W);
			dup(q[W]);
			close(q[W]);
			//close(W);
			//dup2(fd, W);
			//close(fd);
	//		printf("%s: PID %d\n", "run_pr1", getpid());			
			execl("my_cat_wc_m", "my_cat_wc_m", NULL);			
		}
	
		case -1: {
			perror("pr1 fork");
			exit(errno);
		}
		default: {
			break;
		}
	}
	
}

void reader(int q[], int fd) {
	int res;
	switch (res = fork()) {
		case 0: {
			char buff[BUFF_SIZE];
			close(R);
			dup(q[R]);
			close(q[R]);
			
			fgets(buff, BUFF_SIZE, stdin);
			write(fd, buff, BUFF_SIZE);
			exit(EXIT_SUCCESS);
		}
		case -1:
			perror("READER");
			exit(errno);
		default:
		
			break;
	}
}

void write_into_p(int p[], const char * buff)
{
	pid_t res;
	switch (res = fork()) {
		case 0: {
	//		printf("%s: PID %d\n", "write_into_p", getpid());
	// поднменяем стдоаут и пробрасываем через него буфер
			close(W);
			dup(p[W]);
			close(p[W]);
			close(p[R]);
			puts(buff);
			
			exit(EXIT_SUCCESS);
		}
		case -1: {
			perror("WRITE FORK");
			exit(errno);
		}
		default:
			break;
	}
}
