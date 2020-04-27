/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 

void * clientThread(void *server)
{
	printf("In thread\n");
	char message[1000];
	char buffer[1024];
	int clientSocket;
	struct sockaddr_in serverAddr;
	socklen_t addr_size;

	int sockfd, numbytes;
        struct addrinfo hints, *servinfo, *p;
        int rv;
        char s[INET6_ADDRSTRLEN];

	//Create the socket
	clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	//Configure settings of the server address
	//Address family is internet
	serverAddr.sin_family = AF_INET;

	//set port number, using htons function
	serverAddr.sin_port = htons(7799);

	//Set IP address to localhost
	serverAddr.sin_addr.s_addr = inet_addr((char *)server);
	memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

	//Connect the socket to the server using the address
	addr_size = sizeof(serverAddr);
	connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
	strcpy(message, "Hello");

	if( send(clientSocket, message, strlen(message), 0)  < 0)
	{
		printf("Send failed\n");
	}

	//read the message from the server into the buffer
	if(recv(clientSocket, buffer, 1024, 0) < 0)
	{
		printf("Receive failed\n");
	}
	//print the received message
	printf("Data received: %s\n", buffer);
	close(clientSocket);
	pthread_exit(NULL);


}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	pthread_t tid[5];
	int i = 0;
	char addresses[4][8] = {"10.0.4.1", "10.0.3.1", "10.0.2.1", "10.0.1.1"};

	while(i < 4)
	{
		if(pthread_create(&tid[i], NULL, clientThread, (void *)&addresses[i]) != 0)
			printf("Failed to create thread\n");
		i++;
	}
	sleep(20);
	i = 0;
	while(i < 4)
	{
		pthread_join(tid[i++], NULL);
		printf("%d:\n", i);
	}

	/*
	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);

	close(sockfd);
	*/
	return 0;
}
