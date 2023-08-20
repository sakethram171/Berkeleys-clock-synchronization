#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <mutex>

using namespace std;

#define PORT 8000

//Here, we can change the MAX_CLIENTS value based on the requirement. I gave 3 for ease of testing
#define MAX_CLIENTS 5


//To store no of clients connected
int clientCount = 0;


//Class to store the Client's information like the socked file descriptor, client's logical clock value.
class Client{
	
public:
	int sockFd;
	int logicalTime;
	
};

//Creating array of Client objects to store individual client's data.
Client clients[100];


//Helper function to print the logical clock values of all clients
void printAllLogicalClocks()
{
	
	    cout<<"\n***************************************************************************\n";
		cout<<"\nPrinting Logical times of Clients\n";
		for (int i=0;i<clientCount;i++){
			
			cout<<"\nLogical clock of Client "<<i<<" is "<<clients[i].logicalTime<<"\n";
			
			}
        cout<<"\n***************************************************************************\n";
}

//Helper function to calculate Average of all clock values received from clients
int calcAverageClock(int serverLocalClock)
{
	//initializing avg with daemon's(server's) clock
	int avg = serverLocalClock;
	
	//adding all clock values from clients
	for (int i=0;i<clientCount;i++){
		
		avg = avg + clients[i].logicalTime;
	}
	
	//calculating average
	return avg / (clientCount + 1);
	
}



int main()
{

	int optvalue=1;
	int sockfd, connfd, len;
	struct sockaddr_in serveradd, client;
	pthread_t threads[99999];
	int myLocalClock;
	

	//creating and holding a stream socket file descriptor
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	//verifiying if socket is created successfully or not
	if (sockfd == -1)
	{
		cout<<"Failed to create a socket\n";
		exit(0);
	}
	
	//set all bytes to ZERO
	bzero(&serveradd, sizeof(serveradd));


    //sets options associated with a socket.
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optvalue, sizeof(optvalue)))
	    {
	        cout<<"\nError in setsockopt"<<endl;
	        exit(0);
	    }
		
	//assigning family, address and port to serveradd
	serveradd.sin_family = AF_INET;
	serveradd.sin_addr.s_addr = htonl(INADDR_ANY);
	serveradd.sin_port = htons(PORT);

	// Binding the  address to the socket
	int val;
	val = bind(sockfd, (struct sockaddr *) &serveradd, sizeof(serveradd));
	
	//verifying socket bind
	if (val!= 0)
	{
		cout<<"\nFailed to bind the socket."<<endl;
		exit(0);
	}
	else
		cout<<"Socket successfully binded.\n";

	// Listen on the socket
	val = (listen(sockfd, 5));
	if (val != 0) {
		cout<<"\nListening failed!\n";
		exit(0);
	}
	else
		cout<<"\nServer listening now..\n";
	
      cout<<"\n***************************************************************************\n";
	   
	    //accept incoming connections till forever
		while( clientCount < MAX_CLIENTS)
		{
		
			len = sizeof(client);
			cout<<"\nAccepting after listen\n";
			connfd = accept(sockfd, (struct sockaddr *)&client,(socklen_t*) &len);
			
			if(connfd<0)
				cout<<"\nAccept Failed!"<<endl;
			else
				cout<<"\nAccept Success!"<<endl;
			
			cout<<"\nClient "<<clientCount<<" connected!"<<endl;
			cout<<"\n***************************************************************************\n";
			
			//storing client connfd to request logicaltime in future
			clients[clientCount].sockFd = connfd;

			clientCount++;
		}
		
	
		for (int i=0;i<MAX_CLIENTS;i++)
		{
			//readBuff to read each message send from clients
			char readBuff[256];
			int rd;
			
			//Daemon sending messages to all connected clients to request their respective logical time.
			char message[256]="Hey client, I am a Deamon server. Can you please send me your logical clock value?";
			
			//initialize readBuff with zeros
			bzero(readBuff,256);
			
			cout<<"\nRequesting logical clock value from client "<<i<<"\n";
	    	send(clients[i].sockFd,message,strlen(message),0);
			//cout<<"\n Message sent to client"<<i<<"\n";
			
			//reading message sent from client
			while(rd = read( (long)clients[i].sockFd, readBuff, 256) > 0)
			{
				//splitting the string to read the clock value
				string delimiter = " ";
				size_t pos = 0;
				string word;
				string str;
				cout<<"\nReceived message from client "<<i<<" as :"<<readBuff<<endl;
	
		        cout<<"\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n";
				//storing as string to split into words
				str = string(readBuff);
				
				//storing individual words
				while ((pos = str.find(delimiter)) != std::string::npos)
				{
					word = str.substr(0, pos);
					str.erase(0, pos + delimiter.length());
				}
				
               //cout<<str;
				
				clients[i].logicalTime = atoi(str.c_str());
				break;
			}
				
					
		}
		
		//randomly initializing server's logical clock between 0 and 24.(adding 3 to get more randomized values)
		//srand sets the seed value for random generator
		srand(time(NULL));
		myLocalClock = (rand() % 24+ 3)%24;
		
		cout<<"\nMy(Daemon's) logical clock is : "<<myLocalClock<<endl;

        //printing logical clock values of all the clients
		printAllLogicalClocks();
		
		//calculating average 
		int avgClock = calcAverageClock(myLocalClock);
		
	    cout<<"\nAverage clock value calculated at Daemon is : "<<avgClock<<endl;
          
		cout<<"\n***************************************************************************\n";
		 
		cout<<"\nSending clock offset values to clients\n";
		for (int i=0;i<MAX_CLIENTS;i++)
		{
			//calculating offset value for each client to send them back so that they can adjust their time
            int clockOffset = avgClock - clients[i].logicalTime;
			string messageCD;
			
			//Constructing message
			string str1 = "Clock offset is ";
			string str2 = std::to_string(clockOffset);
			
			//messageCD has the offset value which is sent to each client
			messageCD = str1 + str2;
			
			//converting string to char array
			char buffermsgCD[256];
			strcpy(buffermsgCD, messageCD.c_str());
			
			//cout<<"\nSending clock offset value to client "<<i<<"\n";
			
			//sending offset value to client i
	    	send(clients[i].sockFd,buffermsgCD,strlen(buffermsgCD),0);
	     	//cout<<"\n Clock difference sent to client"<<i<<"\n";
		
		}
		
		//Adjusting server's local clock value by calculating offset
		cout<<"\nAdjusting my local clock as per my offset value.\n";
		int serverClockOffset = avgClock - myLocalClock;
		myLocalClock = myLocalClock + serverClockOffset;
		
		cout<<"\n******************** After adjustment, CLOCK AT DAEMON is: "<<myLocalClock<<" *********************"<<endl;

	//closing the newly created socket
	close(sockfd);
}


// NOTE : Used code from my Project 1.