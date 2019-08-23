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
#include <ctype.h>

#define MYPORT 21837

//Defining Structures
//Structure to store result from the AWS
struct received_data{
	char function[10];
	char word[27];
} result;

//structure to store result which is sent to AWS
struct send_data{
	int count[3];
	char searchWord[3000];
	char prefixWord[3000];
	char suffixWord[3000];
	char specialWord[3000];
	char specialMeaning[3000];
} data;

//Main function
int main(void){
//initializations
struct received_data result;//store received data
struct send_data data;//store send result
	
//Socket setup
int serverA_socket;
struct sockaddr_in serverA_address;
struct sockaddr_storage client_information;
if ((serverA_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
{
	perror("Error while creating socket");
	exit(1);
}

//Server address setup: Reference:Beej Section 5.3
serverA_address.sin_family = AF_INET;
serverA_address.sin_port = htons(MYPORT);//server A on port 21837
serverA_address.sin_addr.s_addr=inet_addr("127.0.0.1");
memset(serverA_address.sin_zero,'\0',sizeof serverA_address.sin_zero);//clear memory for server address

//Binding the server address to the socket
if(bind(serverA_socket, (struct sockaddr *)&serverA_address, sizeof(serverA_address))<0)
{
	close(serverA_socket);
	perror("Error while Binding socket");
	exit(2);
}

//Backend server data file(different pointers to perform different opertions:search,prefix,suffix)
FILE *fptr;
char* filename = "backendA.txt";
FILE *fptr1;
char* filename1 = "backendA.txt";
FILE *fptr2;
char* filename2 = "backendA.txt";

//server ready 
printf( "\nThe Server A is up and running using UDP on port %d.\n", MYPORT);

//Main while loop
while(1)
{

//opening file
if ((fptr = fopen(filename, "r")) == NULL)
{
      	printf("\nCould not open file %s\n",filename);
       	exit(0);
}
if ((fptr1 = fopen(filename1, "r")) == NULL)
{
       	printf("\nCould not open file %s\n",filename1);
       	exit(0);
}
if ((fptr2 = fopen(filename2, "r")) == NULL)
{
       	printf("\nCould not open file %s\n",filename2);
       	exit(0);
}

//Receving data from AWS
int p=sizeof(client_information);
if(recvfrom(serverA_socket,(struct received_data *)&result, sizeof(result),0,(struct sockaddr *)&client_information, &p)<0)
{
	perror("Error recieving data:");
	exit(3);
}
printf("\nThe ServerA received input <%s> and operation <%s>\n",result.word,result.function);
	
//Processing data
//initializations for processing data
char* line = NULL;
size_t len = 0;
ssize_t read;
char break_at[] = " ";
char *key=NULL;
char *c=NULL;
int countForSearch=0;
int countForPrefix=0;
int countForSuffix=0;
int countForSimilar=0;
char fullWord[27]="";
strcpy(fullWord,result.word);
strcat(fullWord," ");//the entire word	
char word2[27]="";
strcpy(word2,result.word);
word2[0]=tolower(word2[0]);

//For search and prefix
while ((read = getline(&line, &len, fptr)) != -1) 
{
	if(strncmp(line,result.word,strlen(result.word))==0)
	{
		//search for full word
		if(strncmp(line,fullWord,strlen(fullWord))==0)
		{		
			strcat(data.searchWord,line);
			strcat(data.searchWord,"\n");
			countForSearch++;
		}
		//Match Prefix
		char *inputLine = (char*) calloc(strlen(line) + 1, sizeof(char));
		strncpy(inputLine, line, strlen(line));
		key = strtok_r(inputLine,break_at,&c);					
		strcat(data.prefixWord,key);
		strcat(data.prefixWord,"\n");
		countForPrefix++;
	}
	else
	{
		continue;
	}
}
free(line);

//For suffix
char* line_1= NULL;
size_t xyz = 0;
ssize_t read1;	
while ((read1 = getline(&line_1, &xyz, fptr1)) != -1)
    {
        char *len1=strtok(line_1," :: ");
        char *word=len1+strlen(len1)-strlen(result.word);
        if((strcmp(result.word,word)==0)||(strcmp(word,word2)==0))
        {
            //printf("Word: %s\n",len1);
		strcat(data.suffixWord,len1);
		strcat(data.suffixWord,"\n");
		countForSuffix++;		
            //char *len3=strtok(NULL,"");
            //printf("Meaning :%s\n",len3);
        }
    }
free(line_1);

//For special function for monitor
if(strcmp(result.function,"search")==0)
{
	char* line_2= NULL;
	size_t abc = 0;
	ssize_t read2;	
	int k=0;
	while ((read2 = getline(&line_2, &abc, fptr2)) != -1)
	{
		char *len2=strtok(line_2," :: ");
		if(strlen(len2)==strlen(result.word))
		{
			int cnt=0;
			for(k=0;k<strlen(result.word);k++)
			{
				if(len2[k]==result.word[k])
				{
					cnt++;
				}
			}
			if(cnt==(strlen(result.word)-1))
			{
				//printf("%s",len2);
				if(strcmp(data.specialMeaning,"")==0){
				strcat(data.specialWord,len2);
				char *len5=strtok(NULL,"");
				strcat(data.specialMeaning,len5);
				}
				countForSimilar++;
				//break;
			}
		}
	}
free(line_2);
}
//Copying results to structure
data.count[0]=countForSearch;
data.count[1]=countForPrefix;
data.count[2]=countForSuffix;
if(strcmp(result.function,"search")==0){
printf("\nThe serverA has found <%d> match and <%d> similar words.\n",data.count[0],countForSimilar);
}
if(strcmp(result.function,"prefix")==0){
printf("\nThe serverA has found <%d> matches.\n",data.count[2]);
}
if(strcmp(result.function,"suffix")==0){
printf("\nThe serverA has found <%d> matches.\n",data.count[3]);
}

//sending data
if (sendto(serverA_socket,(struct send_data *)&data,sizeof(data),0,(struct sockaddr *)&client_information,sizeof(client_information)) < 0)
{
	perror("send error");
	close(serverA_socket);
	exit(4);
}	
printf("\nThe Server A finished sending the output to AWS\n");
//reset structure
//printf("data sent");
memset(&result,'\0',sizeof result);
memset(&data,'\0',sizeof data);
//break;

//close file
fclose(fptr1);
fclose(fptr);
fclose(fptr2);		
}

//close socket
//close(serverA_socket);

return 0;
}


