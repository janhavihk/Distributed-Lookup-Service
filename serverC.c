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

#define MYPORT 23837

//Defining Structure
//structure to store the received data from AWS
struct received_data{
	char function[10];
	char word[27];
} result;//store result

//structure to store the results
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

//socket setup	
int serverC_socket;
struct sockaddr_in serverC_address;
struct sockaddr_storage client_information;
	
//create socket
if ((serverC_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
{
	perror("Error while creating socket");
	exit(1);
}
//Server address setup: Reference:Beej Section 5.3
serverC_address.sin_family = AF_INET;
serverC_address.sin_port = htons(MYPORT);//server C on port 23837
serverC_address.sin_addr.s_addr=inet_addr("127.0.0.1");
memset(serverC_address.sin_zero,'\0',sizeof serverC_address.sin_zero);//clear memory for server address

//Binding the server address to the socket
if(bind(serverC_socket, (struct sockaddr *)&serverC_address, sizeof(serverC_address))<0)
{
	close(serverC_socket);
	perror("Error while Binding socket");
	exit(2);
}

//open file
FILE *fptr;
char* filename = "backendC.txt";
FILE *fptr1;
char* filename1 = "backendC.txt";
FILE *fptr2;
char* filename2 = "backendC.txt";

//server ready 
printf( "The Server C is up and running using UDP on port %d.\n", MYPORT);


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

//receiving data
int p=sizeof(client_information);
if(recvfrom(serverC_socket,(struct received_data *)&result, sizeof(result),0,(struct sockaddr *)&client_information, &p)<0)
{
	perror("Error recieving data:");
	exit(3);
}
printf("The ServerC received input <%s> and operation <%s>",result.word,result.function);	
//processing data
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

//for search and prefix		
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
		key = strtok_r (inputLine, break_at, &c);
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
//for suffix
char* line_1= NULL;
size_t xyz = 0;
ssize_t read1;	
int lengthofstring=strlen(result.word);
while ((read1 = getline(&line_1, &xyz, fptr1)) != -1)
    {
        char *len1=strtok(line," :: ");
        char *word=len1+strlen(len1)-lengthofstring;
        if((strcmp(result.word,word)==0)||(strcmp(word,word2)==0))
        {
            //printf("Word: %s\n",len1);
		strcat(data.suffixWord,len1);
		strcat(data.suffixWord,"\n");
		countForSuffix++;		
            char *len3=strtok(NULL,"");
            //printf("Meaning :%s\n",len3);
        }
    }
free(line_1);		
//for special function of monitor
if(strcmp(result.function,"search")==0){
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
//printf("out of while loop for special word");
free(line_2);
}

//copying data to structure
data.count[0]=countForSearch;
data.count[1]=countForPrefix;
data.count[2]=countForSuffix;
if(strcmp(result.function,"search")==0){
printf("The serverA has found <%d> match and <%d> similar words",data.count[0],countForSimilar);
}
if(strcmp(result.function,"prefix")==0){
printf("The serverC has found <%d> matches",data.count[2]);
}
if(strcmp(result.function,"suffix")==0){
printf("The serverC has found <%d> matches",data.count[3]);
}

//sending data
//printf("\nprefix results:\n%s\n search result:\n %s\n",data.prefixWord,data.searchWord);
if (sendto(serverC_socket,(struct send_data *)&data,sizeof(data),0,(struct sockaddr *)&client_information,sizeof(client_information)) < 0)
{
	perror("send error");
	close(serverC_socket);
	exit(4);
}	
printf("\nThe Server C finished sending the output to AWS\n ");
//printf("data sent");
//reset structure
memset(&result,'\0',sizeof result);
memset(&data,'\0',sizeof data);
//break;
		
//close file
fclose(fptr);
fclose(fptr1);
fclose(fptr2);
}
//close socket
//close(serverC_socket);////////////////change

return 0;
}


