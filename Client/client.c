// Lab 8: echo-socket/client.c - simple TCP synchronous sockets - client

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include<time.h>		// for clock() - a standard library function
#include <sys/time.h>		// gettimeofday() - a system call

#define INPUTSIZ 256

//Function to display the menu options to the client
void displaymenu()
{
    printf("\n-----MENU OPTIONS-----\n");
    printf("1. Display Student ID\n");
    printf("2. Display System Information\n");
    printf("3. Display Files In The Server\n");
    printf("4. Copy A File From The Server\n");
    printf("5. Exit\n");
    printf("----------------------\n");
}


int main(void)
{
    int sockfd = 0;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
	perror("Error - could not create socket");
	exit(EXIT_FAILURE);
    }
    serv_addr.sin_family = AF_INET;

    // IP address and port of server we want to connect to - probably best not to hardcode...
    serv_addr.sin_port = htons(50021);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // try to connect...
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
	perror("Error - connect failed");
	exit(EXIT_FAILURE);
    } else
	printf("Connected to server.\n");

    struct timeval tv1, tv2; //varables to track the start and finish times of execution
//Tracks the execution start time
    // get "wall clock" time at start
    if (gettimeofday(&tv1, NULL) == -1) {
	perror("gettimeofday error");
	exit(EXIT_FAILURE);
    }
    // set CPU time at start
    clock_t start, end;
    if ((start = clock()) == -1) {
	perror("clock start error");
	exit(EXIT_FAILURE);
    }



    unsigned char send_buff[INPUTSIZ];
    unsigned char recv_buff[INPUTSIZ];

    char input[INPUTSIZ];

    // get welcome message
    // be careful with the semantics of read() and write()!
    //  read() will block if no data is available, however if data is available
    //  read() will read *up to* recv_buff bytes and then return if
    //  no further data is currently available; this might happen if:-
    //   - the incoming data has now ended because it was of a smaller size *or*
    //   - further data has been delayed by network buffering
    //  also it's possible there are still more bytes to read than recv_buff bytes

     displaymenu(); //displays the menu options to the user

    int bytes_received = read(sockfd, recv_buff, sizeof(recv_buff));
    printf("bytes received: %d\n", bytes_received);
    printf("%s\n", (char *) recv_buff);

    while (1) {
	// some console input from user
	fgets(input, INPUTSIZ, stdin);
	input[strcspn(input, "\n")] = 0;

	// send it to server
	strcpy((char *) send_buff, input);
	write(sockfd, send_buff, strlen((char *) send_buff) + 1);	// send it even if its just '\0' for an empty string

if(strcmp(input,"4")==0){ //if the user select option 4 it asks fot the user input again for the name of the file they want
    printf("Enter a file to copy: ");
    fgets(input, INPUTSIZ, stdin); //gets input from the user so it can be sent to the server
input[strcspn(input, "\n")] = 0;
strcpy((char *) send_buff, input);
	write(sockfd, send_buff, strlen((char *) send_buff) + 1);
 displaymenu(); //displays the menu again
}else
if(strcmp(input,"5")==0) //Shuts down Client if user selected option 5 (exit)
    break;
else
if (strcmp(input,"5")!=0) //displays the menu again if the user does not decide to exit
displaymenu();

	// get some data back - this will block if nothing there yet
	read(sockfd, recv_buff, sizeof(recv_buff));
	printf("%s\n", (char *) recv_buff);
    }

//Gets the end of execution time
 // set CPU time at end
    if ((end = clock()) == -1) {
	perror("clock end error");
	exit(EXIT_FAILURE);
    }


    // get "wall clock" time at end
    if (gettimeofday(&tv2, NULL) == -1) {
	perror("gettimeofday error");
	exit(EXIT_FAILURE);
    }
    // Calculates the execution time and displays it in the client when it finishes execution

    printf("\nTotal execution time = %f seconds\n",
	   (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	   (double) (tv2.tv_sec - tv1.tv_sec));


    // always clean up sockets gracefully
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);

    exit(EXIT_SUCCESS);
}
