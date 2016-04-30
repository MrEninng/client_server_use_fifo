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


void reader(int q[], char *buff, char *privFifo);
void run_pr1(int p[], int q[]);
void write_into_p(int p[], const char * buff);

int main(int argc, char **argv)
{
	char myFifo2[BUFF_SIZE] = "/tmp/myfifo2.fifo";

	puts("I'm SERVER");
	
	int fifo_fd;	
	unlink(PUB_FIFO);
	unlink(myFifo2);
	
	if (mkfifo(PUB_FIFO, 0666) < 0) {
		perror("mkfifo failed");
		exit(errno);
	}
	if (mkfifo(myFifo2, 0666) < 0) {
		perror("mkfifo2 failed");
		exit(errno);
	}
	int res = -1;
	char pass[BUFF_SIZE];
	char passwordBuff[BUFF_SIZE];
	//sleep(3);
	puts("Hi, Admin. Input password pls.");
	fgets(pass, BUFF_SIZE, stdin);
	printf("Thanks. Yor pswd: %s", pass);
	puts("Waiting for client...");
	
	int clientFlag = 1;
	while(clientFlag) {
		
		int fd2 = open(PUB_FIFO, O_RDONLY);
		res = read(fd2, passwordBuff, BUFF_SIZE);
		if (res < 0 ) {
			printf("????");
		} 
		close(fd2);
		res  = strcmp(passwordBuff, pass);
		if ( res != 0) {
			puts("Client input not correct pswd. \nWaiting client...");
			int fd3 = open(PUB_FIFO, O_WRONLY);
			write(fd3, "1\0",BUFF_SIZE);
			close(fd3);
		} else {
			puts("Client connected");
			clientFlag = 0;
			int fd3 = open(PUB_FIFO, O_WRONLY);
			write(fd3, "0\0",BUFF_SIZE);
			close(fd3);
			puts("Send fifo chanel address");
			sleep(1); // NEED TO SYNC WITH CLIENT
			int fd = open(PUB_FIFO, O_WRONLY);
			write(fd, myFifo2, sizeof(myFifo2));
			close(fd);
		}
	}
	char buff[BUFF_SIZE];

	while (1) {
		fifo_fd = open(myFifo2, O_RDONLY);
		int res = read(fifo_fd, buff, BUFF_SIZE);
		//puts(buff);
		if (res < 0) {
			perror("Closed fifo1");
			exit(errno);
		}
		
		int p[2], q[2];
		pipe(p);
		pipe(q);
		write_into_p(p, buff);
		close(p[W]);
		run_pr1(p, q);
		close(p[R]);
		reader(q, buff, myFifo2);
		//puts(buff);
		close(q[R]);
		close(q[W]);
		int status;
		pid_t pid;
		while ((pid = wait(&status)) != -1);
		close(fifo_fd);
		
		//int fd = open(myFifo2, O_WRONLY);
	//	write(fd, buff, BUFF_SIZE);
//		close(fd);
	}  
	unlink(myFifo2);
	unlink(PUB_FIFO);
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
	//		printf("%s: PID %d\n", "run_pr1", getpid());			
			execl("add_space_filter", "add_space_filter", NULL);			
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

void reader(int q[], char* buff, char* privFifo)
{
	int res;
	switch (res = fork()) {
		case 0: {
			char buff[BUFF_SIZE];
			close(R);
			dup(q[R]);
			close(q[R]);
			
			fgets(buff, BUFF_SIZE, stdin);
				//const char *myFifo2 = "/tmp/myfifo2.fifo";
			int fd = open(privFifo, O_WRONLY);
			write(fd, buff, BUFF_SIZE);
			close(fd);
			exit(EXIT_SUCCESS);
		}
		case -1:
			perror("READER");
			exit(errno);
		default:
			break;
	}
}



