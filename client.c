//Header files
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include<ctype.h>
#define SERVER_PORT 25837

//Defining structures to take input from client and store output received from the AWS
struct results_to_client_data{
	int value;
	char output[9000];
} solution;

struct input{
	char function[10];
	char word[27];
} packet;

//Main function
int main(int argc, char* argv[]){
//check user input validity
if(argc!=3)
{
	perror("Invalid Input(check input arguments and try again)\n");
	exit(1);
}

//Initializations for establishing TCP connection with the AWS
int clientsocket;
struct sockaddr_in awsaddr, clientaddr;//structure to store server and client address information 
struct input packet;//for storing input data from client

//making sure inital letter is capital,as we will be comparing the input word with the key values in the dictionary which has initial capital letter
char abc[27];
strcpy(abc,argv[2]);
abc[0]=toupper(abc[0]);

//input stored to structure
strcpy(packet.function,argv[1]);
strcpy(packet.word,abc);

//client address setup: Reference:Beej Section 5.3
clientaddr.sin_family=AF_INET;
clientaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
clientaddr.sin_port=htons(0);	//dynamic port assignment

//Server address setup: Reference:Beej Section 5.3
awsaddr.sin_family = AF_INET;
awsaddr.sin_port = htons(SERVER_PORT);//AWS server on port 25837
awsaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
memset(awsaddr.sin_zero,'\0',sizeof awsaddr.sin_zero);//clear memory for server address

//Socket setup
//(1)Create Socket
if ((clientsocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
{
	perror("Error while creating socket");
	exit(3);
}

//Binding the client address to the socket
if(bind(clientsocket, (struct sockaddr *)&clientaddr, sizeof (clientaddr))<0)
{
	close(clientsocket);
	perror("Error while Binding socket");
	exit(4);
}

//(2)Connect socket to server
if (connect(clientsocket, (struct sockaddr *)&awsaddr, sizeof(awsaddr)) < 0)
{
	perror("\nError while connecting\n");
	exit(5);
}

//client socket is ready	
printf("\nThe client is up and running.\n");
	
/*// Get my ip address and port
char myIP[16];
unsigned int myPort;
int len = sizeof(clientaddr);
getsockname(clientsocket, (struct sockaddr *) &clientaddr, &len);
inet_ntop(AF_INET, &clientaddr.sin_addr, myIP, sizeof(myIP));
myPort = ntohs(clientaddr.sin_port);
printf("Local ip address: %s\n", myIP);
printf("Local port : %u\n", myPort);
*/

//send and receive data
while(1)
{	//sending input data as structure to AWS
	if(send(clientsocket,&packet,sizeof(packet),0)<0)
	{
		perror("\nError while sending packet\n");
		exit(6);
	}
	printf("\nThe client sent < %s > and < %s > to AWS\n",packet.word,packet.function);
	//receiving solution from AWS
	if(recv(clientsocket,&solution,sizeof(solution),0)<0)
	{
		perror("\nError while recieving packet\n");
		exit(7);
	}
	//printing result messages
	if(solution.value !=0)
	{
		if((strcmp(packet.function,"prefix")==0)||(strcmp(packet.function,"suffix")==0))
		{
		printf("\nFound < %d > matches for < %s >:\n%s\n",solution.value,packet.word,solution.output);
		}
		else
		{
		printf("\nFound a match for <%s>:\n%s",packet.word,solution.output);}
	}
	else
	{
		printf("\nFound no matches for <%s>\n",packet.word);
	}	
	break;//for client to terminate
}
//release memory(specifically not needed) 
memset(&solution,'\0',sizeof solution);
memset(&packet,'\0',sizeof packet);
//close the client socket
close(clientsocket);
return 0;
}


