#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <arpa/inet.h>
#include <vector>
#include <cstring>
#define PORT 8000
using namespace std;


int main()
{
	
	//initializing variables
	int optvalue =1;
	char readBuff[256];
	char message[256];
	string str;
	int rd=0;
	int sockfd, connfd; 
	struct sockaddr_in serveradd, client;
	int myLogicalClock=0;
	
	
	//creating and holding a stream socket file descriptor
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	//sets options associated with a socket.
	setsockopt((sockfd), SOL_SOCKET, SO_REUSEPORT, &optvalue, sizeof(optvalue));
	
	//Verifiying if socket is created successfully or not
	if (sockfd == -1)
	{
		cout<<"Failed to create a socket\n";
		exit(0);
	}

	//Set all bytes to ZERO
	bzero(&serveradd, sizeof(serveradd));

	//assigning family, address and port to serveradd
	serveradd.sin_family = AF_INET;
	//assigning local IP address
	serveradd.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	serveradd.sin_port = htons(PORT);

	// connecting the client socket to server socket
	connfd = connect(sockfd, (struct sockaddr *) &serveradd, sizeof(serveradd));
	
	//checking if connection was succeeded
	if (connfd!= 0)
	{
		cout<<"Connection with the server failed.\n";
		exit(0);
	}
	else
	{
	cout<<"Wohoooo! Connected to the server.\n";
	}
	
	//initializing readBuff and message with zeros to read and send messages
	bzero(readBuff,256);
	bzero(message,256);
	
	//initialzing clock with random intergers between 0-24, srand() is used to seed the random number generator to produce different values every time
	srand(time(NULL));
	myLogicalClock = rand() % 24;
	
	
	//Receiving message from Daemon
	while(rd = read(sockfd, readBuff,256) >0)
	{
	
	//constructing message to send the local clock value to Daemon
	string str1 = "My local clock value is ";
    string str2 = std::to_string(myLogicalClock);
	str= str1 + str2;
	strcpy(message,str.c_str());
    
	cout<<"\n"<<message<<endl;
	
	cout<<"\nSending clock value to Daemon!"<<endl;
	//sending message to Daemon with client's clock value
	send (sockfd, message, strlen(message),0);
	sleep(1);
	break;
	
	}
	
	//assigning readBuff with zeros to read new message from daemon
	bzero(readBuff,256);
	
	
	int myClockOffset;
	
	//reading clockoffset value sent from daemon to adjust the local clock value of client.
	while(rd = read(sockfd, readBuff,256) >0)
	{
		       cout<<"\nReceived  message from Deamon as : "<<readBuff<<endl;
		
				string delimiter = " ";
				size_t pos = 0;
				string word;
				string str;
		
				//converting readBuff into string
				str = string(readBuff);
				
				//splitting individual words from received string and storing clock offset value
				while ((pos = str.find(delimiter)) != std::string::npos)
				{
					word = str.substr(0, pos);
					str.erase(0, pos + delimiter.length());
				}
				
			   myClockOffset = atoi(str.c_str());
				
				//cout<<"\nMy clock adjustment value received from client : "<<myClockOffset<<endl;
				break;
	}
	
	//Adjusting client's clock using offset sent from daemon
	
	cout<<"\nAdjusting my local clock as per the offset received from Daemon.\n";
	myLogicalClock = myLogicalClock + myClockOffset;
	cout<<"\n*************** CLOCK AFTER ADJUSTMENT: "<<myLogicalClock<<" ********************"<<endl;
	
	 //closing the socket
	 close(sockfd);
	    
	 return 0;
}

