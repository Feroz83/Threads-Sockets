#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/time.h>

#define INPUTSIZ 256


int main(void)
{
    // socket setup for connection-oriented TCP synchronous full duplex sockets - always the same basically
    // 2 sockets - a listening one and one for the actual connection
    int listenfd = 0, connfd = 0;

    struct sockaddr_in serv_addr;
    struct sockaddr_in client_addr;	// will contain info about the client when connected
    socklen_t socksize = sizeof(struct sockaddr_in);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    // listen on all network interfaces (may have > 1 in a server etc.)
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // listen on port 50021
    serv_addr.sin_port = htons(50021);

    bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    // all calls to sockets including read() and write() should have their return
    //  values checked like this and errors printed with perror() etc.;
    //  for brevity I have not done this but you should!
    if (listen(listenfd, 10) == -1) {
	perror("Failed to listen");
	exit(EXIT_FAILURE);
    }
    // end socket setup



    // in other words send and receive raw bytes
    unsigned char send_buff[INPUTSIZ];
    unsigned char recv_buff[INPUTSIZ];


//function to get student ID & Time as a string aand send it to the client
void studentID(){

    time_t t;    // always look up the manual to see the error conditions
    //  here "man 2 time"
    if ((t = time(NULL)) == -1) {
	perror("time error");
	exit(EXIT_FAILURE);
    }

    // localtime() is in standard library so error conditions are
    //  here "man 3 localtime"
    struct tm *tm;
    if ((tm = localtime(&t)) == NULL) {
	perror("localtime error");
	exit(EXIT_FAILURE);
    }


char idTime[80]; //varable to hold the student ID and time in

strcpy(idTime, "\nS1429795 "); //Adds student ID to the variable
strcat(idTime, asctime(tm)); //concatenates the student ID and Time together as a single string

strcpy((char *) send_buff, idTime); //sends the data of the student ID & time to the send buffer
write(connfd, send_buff, strlen((char *) send_buff) + 1); //writes the id & time to the client to view
}



//Function to get the system information of the server and send to client
void display_sysinfo(){
    struct sysinfo sys;

//handles errors
    if (sysinfo(&sys) == -1) {
	perror("sysinfo error");
	exit(EXIT_FAILURE);
    }
    struct utsname uts;

    if (uname(&uts) == -1) {
	perror("uname error");
	exit(EXIT_FAILURE);
    }


//two varables one will be concatenating all the details into one string
//The other will temporaryly hold a single piece of info that needs concatenating
char system[80]; 
char system2[80];

   /* sprintf(system, "Uptime(s):          %ld\n", sys.uptime);

    sprintf(system2, "Free RAM(bytes):    %lu\n", sys.freeram);
 
strcat(system, system2);

    sprintf(system2, "Number processes:   %u\n", sys.procs);

strcat(system, system2);

    sprintf(system2, "Last minute load:   %ld\n", sys.loads[0]);
strcat(system, system2);*/

sprintf(system, "System Information(server)\n\n"); //heading
strcat(system, system2); //concatenates it into varable

    sprintf(system2, "Node name:   %s\n", uts.nodename);//nodename
strcat(system, system2);//concatenates it into varable

    sprintf(system2, "System name:   %s\n", uts.sysname); //system name
strcat(system, system2);//concatenates it into varable

    sprintf(system2, "Release:   %s\n", uts.release); //release info
strcat(system, system2);//concatenates it into varable

    sprintf(system2, "Version:   %s\n", uts.version);//version
strcat(system, system2);//concatenates it into varable

    sprintf(system2, "Machine:   %s\n", uts.machine); //machine info
strcat(system, system2);//concatenates it into varable


strcpy((char *) send_buff, system); //sends the system information that was concatenated to the send buffer
 
write(connfd, send_buff, strlen((char *) send_buff) + 1); //writes the sendbuffer to the client to view

}



//Function to view files on the server
void viewFiles(){

char fileName[80]; //varable holds the list of file names on the server

strcpy(fileName,"\0"); //initalises the variable


    DIR *mydir;
    if ((mydir = opendir("../Server/transfer")) == NULL) { //opens the director in the server's transfer folder and checks if it exists
	perror("error");
	exit(EXIT_FAILURE);
    }

    struct dirent *entry = NULL;


    // returns NULL when dir contents are all processed
    while ((entry = readdir(mydir)) != NULL) //goes through each file in the folder as a loop
	{sprintf(fileName,"%s\n", entry->d_name); //stores the current entry of filename info the filename variable

        strcat((char *) send_buff, fileName); } //concatenates the file names into the send buffer
        write(connfd, send_buff, strlen((char *) send_buff) + 1); //writes the file name list to the send buffer for the client to view

}


//variable to transfer a file from the server to the client
void transfer(){

FILE * fpRead; //file pointer to read the file from the client to be transfered
FILE * fpWrite; //file pointer to write a new file with the contents of the transfer file


read(connfd, recv_buff, sizeof(recv_buff)); //reads the input from user, needs to get a filename to move
if (strlen((char *) recv_buff) == 0); //gets input even if its zero

char filename[30]; //stores the name of the file (what the user typed in for filename)

strcpy(filename,(char *) recv_buff); //copies the users input into the variable


char filepath[30]; //stores the path of the file

strcpy(filepath,"../Server/transfer/"); //varable is set to the path of the transfer folder
strcat(filepath,filename); //the path is concatenated to add the selected filename to the path


if( access( filepath, F_OK ) != -1 ) { //only tries to read file if it exists on the server


fpRead = fopen(filepath, "r"); //opens the file from the transfer folder

char writepath[30]; //varable to hold the path that a new file will be written to
strcpy(writepath,"../Client/transfer/"); //the path of the client transfer folder is placed in the variable
strcat(writepath,filename); //the filename is concatinated in that path so the new file has the same name as the one on the server
fpWrite = fopen(writepath,"w+"); //opens the write path



char line[150]; //varable to hold each line in the file

while(!feof(fpRead)){ //goes throgh each line in the file until it reaches the end of the file
fgets(line, 150, fpRead); //gets the line it is at
  strcat((char *) send_buff, line); //concatinates the contents in a single string to the send buffer
}

fclose(fpRead); //closes the Read file

       write(connfd, send_buff, strlen((char *) send_buff) + 1); //copies the content of the file to the send buffer so the client will see what is being transfered

 fprintf(fpWrite, "%s",(char *) send_buff); //writes what was sent to the send buffer (which was the file's contents) to a new file
fclose(fpWrite); //closes the Write file
}

else { //gives message to the client if the file doesnt exist on the server

    strcat((char *) send_buff, "\nThe selected file does not exist"); //if the file does not exist a message is display to the client
        write(connfd, send_buff, strlen((char *) send_buff) + 1); //displays message to user

}

}




    // now actually listen - accept() blocks until connection accepted
    // client_addr will then hold info about connected client
    // notice this returns a new socket on a new (random) port
    printf("Waiting for a client to connect...\n");
    connfd = accept(listenfd, (struct sockaddr *) &client_addr, &socksize);


    // display IP address of connected client
    printf
	("Accepted connection from client at: %s on new local port: %d\n",
	 inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

//Start of Execution time tracker ---
    struct timeval tv1, tv2; //varable to keep track of start and finish of program execution

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




    // send welcome message to the client
    strcpy((char *) send_buff, "");
    int bytes_sent = write(connfd, send_buff, strlen((char *) send_buff) + 1);
    printf("bytes sent: %d\n", bytes_sent); //remove this mabe  ////////////////////////////////////


    while (1) {
	// get some data from client - this will block if no data
        //  see other comments in client.c
	read(connfd, recv_buff, sizeof(recv_buff));
	if (strlen((char *) recv_buff) == 0)

          
	printf("Received: %s\n", (char *) recv_buff); //remove this after //////////////////////////////////


//Handles input from client menu, checks if the number matches one from the menu and executes the correct function
if(strcmp( (char *) recv_buff,"1")==0){
studentID();
} else
if(strcmp( (char *) recv_buff,"2")==0){
    display_sysinfo();
}else
if(strcmp( (char *) recv_buff,"3")==0){
    viewFiles();
}else
if(strcmp( (char *) recv_buff,"4")==0){
    transfer();
}else
if(strcmp( (char *) recv_buff,"5")==0){//Shuts down if user selected option 4 (exit)
    break;
}else{ //sends message to the client if the lient never enetered a valid option from the menu
    strcpy((char *) send_buff, "You entered an invalid option!");
write(connfd, send_buff, strlen((char *) send_buff) + 1);
}


	// Clears the send buffer
	strcpy((char *) send_buff, "");
	strcat((char *) send_buff, "");

    }



 // Tracks the finished execution time ---
    if ((end = clock()) == -1) {
	perror("clock end error");
	exit(EXIT_FAILURE);
    }


    // get "wall clock" time at end
    if (gettimeofday(&tv2, NULL) == -1) {
	perror("gettimeofday error");
	exit(EXIT_FAILURE);
    }
    // calculates the time the execution took and displays it in the server after it finishes executing

    printf("\nTotal execution time = %f seconds\n",
	   (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
	   (double) (tv2.tv_sec - tv1.tv_sec));



    // always clean up sockets gracefully
    shutdown(connfd, SHUT_RDWR);
    close(connfd);

    // shutdown not required on listening sockets
    close(listenfd);

    exit(EXIT_SUCCESS);
}
