//header files
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

#define MY_TCP_PORT_CLIENT 25837   //AWS TCP port to client
#define MY_TCP_PORT_MONITOR 26837   //AWS TCP port to monitor
#define MY_UDP_PORT 24837   //AWS UDP port
#define BACKLOG 10	    //Limit to how many clients can connect
#define LOCALHOST "127.0.0.1"	//localhost IP
#define SERVER_A_PORT 21837
#define SERVER_B_PORT 22837
#define SERVER_C_PORT 23837

//Defining Structures
//structure to store input data from client
struct received_data_from_client{
	char function[10];
	char word[27];
} result;
//structure to store data received from backend servers
struct backend_data{
	int count[3];
	char searchWord[3000];
	char prefixWord[3000];
	char suffixWord[3000];
	char specialWord[3000];
	char specialMeaning[3000];
} data;
//structure to send to the client after processing
struct results_to_client_data{
	int value;
	char output[9000];
} solution;
//structure to send to the monitor after processing
struct results_to_monitor_data{
	int value;
	char word[27];
	char output[9000];
	char input[2000];
	char special[3000];
	char specialMeaning[3000];
} solutionMonitor;

//Main Function
int main(){
	
//Initializations to store data from the backend servers
struct backend_data data;//store backend results
int numbAsearch,numbBsearch,numbCsearch,numbCprefix,numbBprefix,numbAprefix,numbCsuffix,numbBsuffix,numbAsuffix;
int valueForSearch,valueForPrefix, valueForSuffix;
	
//initializations
struct results_to_client_data solution;//final result to client
struct results_to_monitor_data solutionMonitor;//final result to monitor
char outputSearch[9000];//search result to client
char outputPrefix[9000];//prefix result to client
char outputSuffix[9000];//prefix result to client

//initialisations for monitor and client socket
int awsTCPsocketmonitor,new_awsTCPsocketmonitor,awsTCPsocketclient,new_awsTCPsocketclient;//TCP Socket for Monitor & client
struct sockaddr_in serverAddrMon,monitor_info,awsaddr,client_info;//structure to store server address information & requesting client address information
struct received_data_from_client result;//structure to store received data from client
socklen_t addr_size,sin_size;
int yes=1;
	
//Socket setup for CLIENT:
//Create  Socket
if ((awsTCPsocketclient = socket(AF_INET, SOCK_STREAM, 0)) < 0)
{
	perror("Error while creating socket");
	exit(1);
}

//making the port reusble:Reference:Beej Section 7.2
setsockopt(awsTCPsocketclient, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

//Server address setup: Reference:Beej Section 5.3
awsaddr.sin_family = AF_INET;
awsaddr.sin_port = htons(MY_TCP_PORT_CLIENT);//AWS server on port 25837
awsaddr.sin_addr.s_addr=inet_addr(LOCALHOST);//localhost IP address
memset(awsaddr.sin_zero,'\0',sizeof awsaddr.sin_zero);//clear memory for server address

//Binding the server address to the socket
if(bind(awsTCPsocketclient, (struct sockaddr *)&awsaddr, sizeof(awsaddr))<0)
{
	close(awsTCPsocketclient);
	perror("Error while Binding socket:TCP");
	exit(3);
}


//Socket setup MONITOR:
//Create Socket
if ((awsTCPsocketmonitor = socket(AF_INET, SOCK_STREAM, 0)) < 0)
{
	perror("Error while creating socket");
	exit(1);
}

//making the port reusble:Reference:Beej Section 7.2
setsockopt(awsTCPsocketmonitor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

//Server address setup: Reference:Beej Section 5.3
serverAddrMon.sin_family = AF_INET;
serverAddrMon.sin_port = htons(MY_TCP_PORT_MONITOR);//AWS server on port 26837
serverAddrMon.sin_addr.s_addr=inet_addr(LOCALHOST);//localhost IP address
memset(serverAddrMon.sin_zero,'\0',sizeof serverAddrMon.sin_zero);//clear memory for server address

//Binding the server address to the socket
if(bind(awsTCPsocketmonitor, (struct sockaddr *)&serverAddrMon, sizeof(serverAddrMon))<0)
{
	close(awsTCPsocketmonitor);
	perror("Error while Binding socket:TCP");
	exit(3);
}

//Listening and accepting connection form the monitor
//Listen to the monitor:Reference: Beej Section 6.1
if (listen(awsTCPsocketmonitor, 1) < 0) 
{
	perror("listening error");
	exit(5);
}

//Accept connection from monitor:Reference: Beej Section 6.1 
addr_size = sizeof(monitor_info);
if ((new_awsTCPsocketmonitor= accept(awsTCPsocketmonitor, (struct sockaddr *)&monitor_info, &addr_size)) <0)
{		
	perror("accept");
	exit(6);
}

//Listening and accepting connection from the CLIENT	
//listen to the client:Reference: Beej Section 6.1
if (listen(awsTCPsocketclient, BACKLOG) < 0) 
{
	perror("listening error");
	exit(5);
}

printf("\nThe AWS is up and running.\n");


//Backend servers socket setup
//SERVER A
int awsclient_socket_to_A;
struct sockaddr_in serverA_addr,awsaddrA;
struct sockaddr_storage aws_infoA;
int p=sizeof(aws_infoA);

//Socket setup for Backend server A
//client address setup: Reference:Beej Section 5.3
awsaddrA.sin_family=AF_INET;
awsaddrA.sin_port=htons(MY_UDP_PORT);	//aws client port to 24837
awsaddrA.sin_addr.s_addr=inet_addr(LOCALHOST);//localhost IP address

//Server address setup: Reference:Beej Section 5.3
serverA_addr.sin_family = AF_INET;
serverA_addr.sin_port = htons(SERVER_A_PORT);//server A on port 21837
serverA_addr.sin_addr.s_addr=inet_addr(LOCALHOST);//localhost IP address
memset(serverA_addr.sin_zero,'\0',sizeof serverA_addr.sin_zero);//clear memory for server address

//Create socket
if ((awsclient_socket_to_A = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
{
	perror("Error while creating socket");
	exit(1);
}

//Binding the client address to the socket
if(bind(awsclient_socket_to_A, (struct sockaddr *)&awsaddrA, sizeof (awsaddrA))<0)
{
	close(awsclient_socket_to_A);
	perror("Error while Binding socket: A");
	exit(3);
}

//SERVER B
int awsclient_socket_to_B;
struct sockaddr_in serverB_addr,awsaddrB;
struct sockaddr_storage aws_infoB;
int q=sizeof(aws_infoB);
//client address setup: Reference:Beej Section 5.3
awsaddrB.sin_family=AF_INET;
awsaddrB.sin_port=htons(26837);	//Error:Address already in use :used port 26837
awsaddrB.sin_addr.s_addr=inet_addr(LOCALHOST);//localhost IP address
	
//create socket
if ((awsclient_socket_to_B = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
{
	perror("Error while creating socket");
	exit(1);
}
//Server address setup: Reference:Beej Section 5.3
serverB_addr.sin_family = AF_INET;
serverB_addr.sin_port = htons(SERVER_B_PORT);//server B on port 22837
serverB_addr.sin_addr.s_addr=inet_addr(LOCALHOST);//localhost IP address
memset(serverB_addr.sin_zero,'\0',sizeof serverB_addr.sin_zero);//clear memory for server address

//Binding the client address to the socket
if(bind(awsclient_socket_to_B, (struct sockaddr *)&awsaddrB, sizeof (awsaddrB))<0)
{
	close(awsclient_socket_to_B);
	perror("Error while Binding socket: B");
	exit(3);
}


//SERVER C
int awsclient_socket_to_C;
	struct sockaddr_in serverC_addr,awsaddrC;
	struct sockaddr_storage aws_infoC;
int r=sizeof(aws_infoC);
//client address setup: Reference:Beej Section 5.3
awsaddrC.sin_family=AF_INET;
awsaddrC.sin_port=htons(27837);	//Error:Address already in use: Solved the error using port 27837
awsaddrC.sin_addr.s_addr=inet_addr(LOCALHOST);//localhost IP address

//create socket
if ((awsclient_socket_to_C = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
{
	perror("Error while creating socket");
	exit(1);
}
//Server address setup: Reference:Beej Section 5.3
serverC_addr.sin_family = AF_INET;
serverC_addr.sin_port = htons(SERVER_C_PORT);//server A on port 21837
serverC_addr.sin_addr.s_addr=inet_addr(LOCALHOST);//localhost IP address
memset(serverC_addr.sin_zero,'\0',sizeof serverC_addr.sin_zero);//clear memory for server address

//Binding the client address to the socket
if(bind(awsclient_socket_to_C, (struct sockaddr *)&awsaddrC, sizeof (awsaddrC))<0)
{
	close(awsclient_socket_to_C);
	perror("Error while Binding socket: C");
	exit(3);
}


//The main accept() loop
while(1) 
{ 
//Accept connection from client:Reference: Beej Section 6.1
sin_size = sizeof(client_info);
if ((new_awsTCPsocketclient= accept(awsTCPsocketclient, (struct sockaddr *)&client_info, &sin_size)) <0)
{
	perror("accept");
	continue;
}
//printf("accepted client\n");
		
//Receive data from client 
if((recv(new_awsTCPsocketclient, (struct received_data_from_client *)&result, sizeof(result), 0))< 0) 
{
	perror("receiving error");
	exit(6);
}
		
printf("\nThe AWS received input=<%s> and function=<%s> from the client using TCP over port <%d>\n", result.word, result.function, MY_TCP_PORT_CLIENT);


//Calling backend server and processing data
//(1)SERVER A
//send &recieve

memset(&data,'\0',sizeof data);//Reset receiving structure
while(1)
{
	if (sendto(awsclient_socket_to_A,&result,sizeof(result),0,(struct sockaddr *)&serverA_addr,sizeof(serverA_addr)) < 0)
	{
		perror("send error");
		close(awsclient_socket_to_A);
		exit(4);
	}	
	printf("\nThe AWS sent <%s> and <%s> to Backend-Server A\n",result.word,result.function);
	if(recvfrom(awsclient_socket_to_A,(struct backend_data *)&data,sizeof(data),0,(struct sockaddr *)&aws_infoA, &p)<0)
	{
		perror("Error recieving data:");
		close(awsclient_socket_to_A);
		exit(3);
	}
	strcat(outputSearch,data.searchWord);
	strcat(outputPrefix,data.prefixWord);
	strcat(outputSuffix,data.suffixWord);
	strcat(solutionMonitor.special,data.specialWord);
	//printf("\n\n\n\nspecial word:\n%s\n\n\n\n\n",data.specialWord);
	strcat(solutionMonitor.specialMeaning,data.specialMeaning);
	numbAsearch=data.count[0];
	//printf("\nAsearch:\n%d\n",numbAsearch);
	//printf("\nAsearch:\n%s\n",data.searchWord);
	numbAprefix=data.count[1];
	//printf("\nAprefix:\n%d\n",numbAprefix);
	//printf("\nAprefix:\n%s\n",data.prefixWord);
	numbAsuffix=data.count[2];
	//printf("\nAsuffix:\n%s\n",data.suffixWord);
	break;
}
//(2)SERVER B
memset(&data,'\0',sizeof data);//Reset receiving structure	
while(1)
{
	if (sendto(awsclient_socket_to_B,&result,sizeof(result),0,(struct sockaddr *)&serverB_addr,sizeof(serverB_addr)) < 0)
	{
		perror("send error");
		close(awsclient_socket_to_B);
		exit(4);
	}	
	printf("\nThe AWS sent <%s> and <%s> to Backend-Server B\n",result.word,result.function);
	if(recvfrom(awsclient_socket_to_B,(struct backend_data *)&data,sizeof(data),0,(struct sockaddr *)&aws_infoB, &q)<0)
	{
		perror("Error recieving data:");
		close(awsclient_socket_to_B);
		exit(3);
	}
	strcat(outputSearch,data.searchWord);
	strcat(outputPrefix,data.prefixWord);
	strcat(outputSuffix,data.suffixWord);
	if(strcmp(solutionMonitor.special,"")==0)
	{
	strcat(solutionMonitor.special,data.specialWord);
	strcat(solutionMonitor.specialMeaning,data.specialMeaning);
	}		
	numbBsearch=data.count[0];
	numbBprefix=data.count[1];
	numbBsuffix=data.count[2];
	break;
}

//(3)SERVER C
memset(&data,'\0',sizeof data);
while(1)
{
	if (sendto(awsclient_socket_to_C,&result,sizeof(result),0,(struct sockaddr *)&serverC_addr,sizeof(serverC_addr)) < 0)
	{
		perror("send error");
		close(awsclient_socket_to_C);
		exit(4);
	}	
	printf("\nThe AWS sent <%s> and <%s> to Backend-Server C\n",result.word,result.function);
	if(recvfrom(awsclient_socket_to_C,(struct backend_data *)&data,sizeof(data),0,(struct sockaddr *)&aws_infoC, &r)<0)
	{
		perror("Error recieving data:");
		close(awsclient_socket_to_C);
		exit(3);
	}
	strcat(outputSearch,data.searchWord);
	strcat(outputPrefix,data.prefixWord);
	strcat(outputSuffix,data.suffixWord);
	if(strcmp(solutionMonitor.special,"")==0)
	{
	strcat(solutionMonitor.special,data.specialWord);
	strcat(solutionMonitor.specialMeaning,data.specialMeaning);
	}
	numbCsearch=data.count[0];
	numbCprefix=data.count[1];
	numbCsuffix=data.count[2];
	break;
}
	
//Sending the received data from the backend servers to the client and monitor
//printf("\nserver done calling and recieving from udp\n");
valueForSearch=numbAsearch+numbBsearch+numbCsearch;//send to client
valueForPrefix=numbAprefix+numbBprefix+numbCprefix;//send to client
valueForSuffix=numbAsuffix+numbBsuffix+numbCsuffix;//send to client

//printf("\nSending data to client\n");
//printf("\nprefixes:%s\nlength:%d\nsearch:%s\nlength:%d\nsuffix:%s\nlength:%d\n",outputPrefix,strlen(outputPrefix),outputSearch,strlen(outputSearch),outputSuffix,strlen(outputSuffix));
if(strcmp(result.function,"search")==0)
{
	printf("\nThe AWS received < %d >   similar words    from   Backend-Server   <   A   >   using   UDP   over   port   < %d >\n",numbAsearch,SERVER_A_PORT);
	printf("\nThe AWS received < %d >   similar words    from   Backend-Server   <   B   >   using   UDP   over   port   < %d >\n",numbBsearch,SERVER_B_PORT);
	printf("\nThe AWS received < %d >   similar words    from   Backend-Server   <   C   >   using   UDP   over   port   < %d >\n",numbCsearch,SERVER_C_PORT);
	solution.value=valueForSearch;
	strcpy(solution.output,outputSearch);
	if ((send(new_awsTCPsocketclient,&solution, sizeof(solution), 0) < 0))
	{
		perror("send error");
		close(new_awsTCPsocketclient);
		exit(0);
	}
	printf("\nThe AWS sent <%d> matches to client.\n",solution.value);
	solutionMonitor.value=valueForSearch;
	strcpy(solutionMonitor.output,outputSearch);
	strcpy(solutionMonitor.word,result.word);
	strcpy(solutionMonitor.input,result.function);
	if((send(new_awsTCPsocketmonitor,&solutionMonitor, sizeof(solutionMonitor), 0) < 0))
	{
		perror("send error");
		close(new_awsTCPsocketmonitor);
		exit(0);
	}
	if(solutionMonitor.special[0] == '\0')
	{
		printf("\nThe AWS sent <%s> and <%s> to the monitor via TCP port <%d>\n",result.word,solutionMonitor.special,MY_TCP_PORT_MONITOR);
	}
}
		
if(strcmp(result.function,"prefix")==0)
{
	printf("\nThe AWS received < %d >   matches   from   Backend-Server   <   A   >   using   UDP   over   port   < %d >\n",numbAprefix,SERVER_A_PORT);
	printf("\nThe AWS received < %d >   matches   from   Backend-Server   <   B   >   using   UDP   over   port   < %d >\n",numbBprefix,SERVER_B_PORT);
	printf("\nThe AWS received < %d >   matches   from   Backend-Server   <   C   >   using   UDP   over   port   < %d >\n",numbCprefix,SERVER_C_PORT);
	solution.value=valueForPrefix;
	strcpy(solution.output,outputPrefix);
	if ((send(new_awsTCPsocketclient,&solution, sizeof(solution), 0) < 0))
	{
		perror("send error");
		close(new_awsTCPsocketclient);
		exit(0);
	}
	printf("\nThe AWS sent <%d> matches to client.\n",solution.value);
	solutionMonitor.value=valueForPrefix;
	strcpy(solutionMonitor.output,outputPrefix);
	strcpy(solutionMonitor.word,result.word);
	strcpy(solutionMonitor.input,result.function);
	if ((send(new_awsTCPsocketmonitor,&solutionMonitor, sizeof(solutionMonitor), 0) < 0))
	{
		perror("send error");
		close(new_awsTCPsocketmonitor);
		exit(0);
	}
	printf("\nThe AWS sent < %d > matches to the monitor via TCP port %d.\n",solutionMonitor.value,MY_TCP_PORT_CLIENT);
}

if(strcmp(result.function,"suffix")==0)
{
	printf("\nThe AWS received < %d >   matches   from   Backend-Server   <   A   >   using   UDP   over   port   < %d >\n",numbAsuffix,SERVER_A_PORT);
	printf("\nThe AWS received < %d >   matches   from   Backend-Server   <   B   >   using   UDP   over   port   < %d >\n",numbBsuffix,SERVER_B_PORT);
	printf("\nThe AWS received < %d >   matches   from   Backend-Server   <   C   >   using   UDP   over   port   < %d >\n",numbCsuffix,SERVER_C_PORT);
	solution.value=valueForSuffix;
	strcpy(solution.output,outputSuffix);
	if ((send(new_awsTCPsocketclient,&solution, sizeof(solution), 0) < 0))
	{
		perror("send error");
		close(new_awsTCPsocketclient);
		exit(0);
	}
	printf("\nThe AWS sent <%d> matches to client.\n",solution.value);
	solutionMonitor.value=valueForSuffix;
	strcpy(solutionMonitor.output,outputSuffix);
	strcpy(solutionMonitor.word,result.word);
	strcpy(solutionMonitor.input,result.function);
	if((send(new_awsTCPsocketmonitor,&solutionMonitor, sizeof(solutionMonitor), 0) < 0))
	{
		perror("send error");
		close(new_awsTCPsocketmonitor);
		exit(0);
	}
	printf("\nThe AWS sent < %d > matches to the monitor via TCP port %d.\n",solutionMonitor.value,MY_TCP_PORT_CLIENT);
}

//reset the structure and few variables used in the while loop
memset(&result,'\0',sizeof result);
memset(&data,'\0',sizeof data);
memset(&solution,'\0',sizeof solution);
memset(&solutionMonitor,'\0',sizeof solutionMonitor);
strcpy(outputSearch,"");
strcpy(outputPrefix,"");
strcpy(outputSuffix,"");
		
//break;

}
//printf("\nserver is out of loop\n");
//close(new_awsTCPsocketclient); // parent doesn't need this
//close(new_awsTCPsocketmonitor); // parent doesn't need this

//close(awsTCPsocket);
}





