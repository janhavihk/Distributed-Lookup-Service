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

#define SERVER_PORT 26837

//defining the structure to take output results from the AWS
struct results_to_client_data{
	int value;
	char word[27];
	char output[9000];
	char input[2000];
	char special[3000];
	char specialMeaning[3000];
} solution;

//Main Function
int main(){

//Socket setup
int monitorsocket;
struct sockaddr_in awsaddr, monitoraddr;

//client address setup: Reference:Beej Section 5.3
monitoraddr.sin_family=AF_INET;
monitoraddr.sin_addr.s_addr=inet_addr("127.0.0.1");
monitoraddr.sin_port=htons(0);	//dynamic port assignment
memset(monitoraddr.sin_zero,'\0',sizeof monitoraddr.sin_zero);

//Server address setup: Reference:Beej Section 5.3
awsaddr.sin_family = AF_INET;
awsaddr.sin_port = htons(SERVER_PORT);//AWS server on port 25837
awsaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
memset(awsaddr.sin_zero,'\0',sizeof awsaddr.sin_zero);//clear memory for server address

//Create Socket
if ((monitorsocket = socket(AF_INET,SOCK_STREAM,0)) < 0)
{
perror("Error while creating socket");
exit(2);
}

//Binding the monitor address to the socket
if(bind(monitorsocket,(struct sockaddr *)&monitoraddr,sizeof (monitoraddr))<0)
{
close(monitorsocket);
perror("Error while Binding socket");
exit(3);
}

/*// Get my ip address and port
char myIP[16];
unsigned int myPort;
int len = sizeof(monitoraddr);
getsockname(monitorsocket, (struct sockaddr *) &monitoraddr, &len);
inet_ntop(AF_INET, &monitoraddr.sin_addr, myIP, sizeof(myIP));
myPort = ntohs(monitoraddr.sin_port);
printf("Local ip address: %s\n", myIP);
printf("Local port : %u\n", myPort);
*/

//(2)Connect socket to server
if (connect(monitorsocket, (struct sockaddr *)&awsaddr, sizeof(awsaddr)) < 0)
{
perror("\nError while connecting\n");
exit(5);
}

//Monitor socket is ready
printf("\nThe monitor is up and running.\n");

//send and receive data
while(1)
{
	if(recv(monitorsocket,&solution,sizeof(solution),0)<0)
	{
		perror("\nError while recieving packet\n");
		exit(7);
	}
	//result messages to be written
	if(strcmp(solution.input,"search")==0)
	{
		printf("\nFound a match for < %s >:\n%s\n",solution.word,solution.output);
		if(strcmp(solution.special,"")!=0)
		{
			printf("\nOne edit distance match is < %s >:\n%s",solution.special,solution.specialMeaning);
		}
	}
	else if((strcmp(solution.input,"prefix")==0)||(strcmp(solution.input,"suffix")==0))
	{
		printf("\nFound <%d> matches for < %s >:\n%s\n",solution.value,solution.input,solution.output);
	}
	else
	{
		printf("\nFound no matches for < %s >\n",solution.input);
	}	

//Reset the structure receiving data
memset(&solution,'\0',sizeof solution);
//break;
}

//close the monitor socket
close(monitorsocket);
return 0;
}


