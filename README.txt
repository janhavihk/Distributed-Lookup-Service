a. Name: Janhavi Hemant Karekar

b. Student ID: 6727039837

c. What I did in the assignment:
	As mentioned in the project,I have completed all the phases including Phase3 that is the suffix part.According to phase 1(a) all the server boot up and then the client and the monitor establish TCP connection with the AWS server and then the client message is sent to the AWS server.
	According to phase 1(b), the AWS send the data received from the client to the backend servers where the backend servers do the processing either search,prefix and suffix and send the respective output to the AWS. The AWS then performs the necessary sorting of the results and give the results to client and the monitor. Also according to phase 2 the backend servers also perform the special function for the monitor and the required result to the AWS.Then AWS then sends this data only to the monitor.
	The data is sent and received by passing structures through the socket. 


d. What my code files are and what each one of them does.
	1.client.c: This code file is responsible to take the input from the user and pass the input through the socket to the AWS.Also it will receive the data from the AWS and display the necessary output to the client.
	2.monitor.c:This code file is responsible for just displaying the data that the AWS sends to the client also it displays one additional word and definition when the input function is "search".
	3.aws.c: This is the center portion of this project. It receives the data from the client on TCP connection and forwards this data to the backend servers(A,B and C) over UDP connection. The results obtained fromt the UDP connection is then manipulated and sorted according to the requirement of the client. After processing the results they are sent to the client and monitor over TCP.
	4.serverA.c: This is responsible to perform the search,prefix, suffix and the special function for monitor on the backendA.txt file.
	5.serverB.c: This is responsible to perform the search,prefix, suffix and the special function for monitor on the backendB.txt file.
	6.serverC.c: This is responsible to perform the search,prefix, suffix and the special function for monitor on the backendC.txt file.

e. The format of all the messages exchanged.
	
For operation ​ search​ :
1.Backend server A terminal:
	
	The Server A is up and running using UDP on port 21837.
	The ServerA received input <Gleek> and operation <search>
	The serverA has found <1> match and <0> similar words.
	The Server A finished sending the output to AWS

2.Backend server B terminal:
	
	The Server B is up and running using UDP on port 22837.
	The ServerB received input <Gleek> and operation <search>
	The serverB has found <0> match and <0> similar words
	The Server B finished sending the output to AWS
	
3.Backend server C terminal:
	
	The Server C is up and running using UDP on port 23837.
	The ServerC received input <Gleek> and operation <search>
	The serverA has found <0> match and <0> similar words
	The Server C finished sending the output to AWS

4.client.c:
	ee450@ee450:~/Desktop$ ./client search gleek
	The client is up and running.
	The client sent < Gleek > and < search > to AWS
	Found a match for <Gleek>:
	Gleek :: "A game at cards  once popular  played by three persons."
	Gleek :: A jest or scoff; a trick or deception.

2.monitor.c
	The monitor is up and running.
	Found a match for < Gleek >:
	Gleek :: "A game at cards  once popular  played by three persons."
	Gleek :: A jest or scoff; a trick or deception.
	One edit distance match is < Greek >:
	:: "A native  or one of the people  of Greece; a Grecian; also  the language of Greece."

For operation Prefix:
	
1.Backend server A terminal:
	
	The Server A is up and running using UDP on port 21837.
	The ServerA received input <Fal> and operation <search>
	The serverB has found <8> match
	The Server A finished sending the output to AWS
	
2.Backend server B terminal:
	The Server B is up and running using UDP on port 22837.
	The ServerB received input <Fal> and operation <prefix>
	The serverB has found <6> match
	The Server B finished sending the output to AWS

3.Backend server C terminal:
	The Server C is up and running using UDP on port 23837.
	The ServerC received input <Fal> and operation <prefix>
	The serverC has found <0> match
	The Server C finished sending the output to AWS

4.aws terminal:
	The AWS received input=<Fal> and function=<prefix> from the client using TCP over port <25837>
	The AWS sent <Fal> and <prefix> to Backend-Server A
	The AWS sent <Fal> and <prefix> to Backend-Server B
	The AWS sent <Fal> and <prefix> to Backend-Server C
	The AWS received < 8 >   matches   from   Backend-Server   <   A   >   using   UDP   over   port   < 21837 >
	The AWS received < 6 >   matches   from   Backend-Server   <   B   >   using   UDP   over   port   < 22837 >
	The AWS received < 0 >   matches   from   Backend-Server   <   C   >   using   UDP   over   port   < 23837 >
	The AWS sent <14> matches to client.
	The AWS sent < 14 > matches to the monitor via TCP port 25837.

4.client terminal:
	ee450@ee450:~/Desktop$ ./client prefix fal
	The client is up and running.
	The client sent < Fal > and < prefix > to AWS
	Found < 14 > matches for < Fal >:
	False
	False
	Falsify
	Falsicrimen
	Falcon
	Faltering
	Falsify
	Falsifier
	Fallowing
	Falchion
	Fall
	Falconet
	Falsify
	Fallopian

5.Monitor terminal:
	The monitor is up and running.
	The client is up and running.
	Found < 14 > matches for < Fal >:
	False
	False
	Falsify
	Falsicrimen
	Falcon
	Faltering
	Falsify
	Falsifier
	Fallowing
	Falchion
	Fall
	Falconet
	Falsify
	Fallopian

For operation Suffix:
	
1.Backend server A terminal:
	The Server A is up and running using UDP on port 21837.
	The ServerA received input <Lse> and operation <suffix>
	The serverA has found <0> matches.
	The Server A finished sending the output to AWS

2.Backend server B terminal:
	The Server B is up and running using UDP on port 22837.
	The ServerB received input <Lse> and operation <suffix>
	The serverB has found <0> match
	The Server B finished sending the output to AWS

3.Backend server C terminal:
	The Server C is up and running using UDP on port 23837.
	The ServerC received input <Lse> and operation <suffix>
	The serverC has found <0> matches
	The Server C finished sending the output to AWS

4.aws terminal:
	The AWS is up and running.
	The AWS received input=<Lse> and function=<suffix> from the client using TCP over port <25837>
	The AWS sent <Lse> and <suffix> to Backend-Server A
	The AWS sent <Lse> and <suffix> to Backend-Server B
	The AWS sent <Lse> and <suffix> to Backend-Server C
	The AWS received < 2 >   matches   from   Backend-Server   <   A   >   using   UDP   over   port   < 21837 >
	The AWS received < 0 >   matches   from   Backend-Server   <   B   >   using   UDP   over   port   < 22837 >
	The AWS received < 0 >   matches   from   Backend-Server   <   C   >   using   UDP   over   port   < 23837 >
	The AWS sent <2> matches to client.
	The AWS sent < 2 > matches to the monitor via TCP port 25837.

4.client terminal;
	ee450@ee450:~/Desktop$ ./client suffix lse
	The client is up and running.
	The client sent < Lse > and < suffix > to AWS
	Found < 2 > matches for < Lse >:
	False
	False

5.Monitor terminal:
	The monitor is up and running.
	Found <2> matches for < suffix >:
	False
	False


g. Any idiosyncrasy of your project. 
	As far as socket programming part is concerned there are no idiosyncrasies.But there is a minor flaw for the "search" results, that if there is one word repeated twice then instead of getting one output,we get both the output.

h. Reused Code: 
	Some part of setting up TCP and UDP are referred from "Socket Programming Reference - (Beej's-Guide).pdf". Those program parts are indicated in the code.
