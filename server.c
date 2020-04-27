/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

char client_message[2000];
char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * socketThread(void *arg)
{
	int newSocket = *((int *)arg);
	recv(newSocket, client_message, 2000, 0);

	//send message to the client socket
	pthread_mutex_lock(&lock);
	char *message = malloc(sizeof(client_message)+20);
	strcpy(message, "Hello Client : ");
	strcat(message, client_message);
	strcat(message, "\n");
	strcpy(buffer, message);
	free(message);
	pthread_mutex_unlock(&lock);
	sleep(1);
	send(newSocket, buffer, 13, 0);
	printf("Exit socketThread \n");
	close(newSocket);
	pthread_exit(NULL);
}

void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size;

	//create the socket
	serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	//configure the settings of the server address struct
	//address family = internet
	serverAddr.sin_family = AF_INET;

	//set port number, using htons function to use proper byte order
	serverAddr.sin_port = htons(7799);

	//set IP addreass
	serverAddr.sin_addr.s_addr = inet_addr("");

	//set all bits of the padding field to 0
	memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

	//bind the address struct to the socket
	bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

	//listen on the socket, with 5 max connection requests queued
	if(listen(serverSocket, 5)==0)
		printf("Listening\n");
	else
		printf("Error\n");
	pthread_t tid[5];
	int i = 0;
	while(1)
	{
		//accept call creates a new socket for the incoming connection
		addr_size = sizeof(serverStorage);
		newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

		//for each client request creates a thread and assign the client request to it to process
		//so the main thread can entertain next request
		if(pthread_create(&tid[i], NULL, socketThread, &newSocket) != 0)
			printf("Failed to create thread\n");

		if( i >= 5)
		{
			i = 0;
			while(i < 5)
			{
				pthread_join(tid[i++], NULL);
			}
			i = 0;
		}
	}
	/*
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	while(1) {  // main accept() loop
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			get_in_addr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		if (!fork()) { // this is the child process
			close(sockfd); // child doesn't need the listener
			if (send(new_fd, "Hello, world!", 13, 0) == -1)
				perror("send");
			close(new_fd);
			exit(0);
		}
		close(new_fd);  // parent doesn't need this
	}
	*/
	return 0;
}
