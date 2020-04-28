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
#include <time.h>
#include <sys/select.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 100 // max number of bytes we can get at once 
#define BILLION 1000000000.0

void * get_in_addr(struct sockaddr *);

double RTT[4];

typedef struct subnet_t {
	int towerNum;
	char *ip;
}subnet;


void * clientThread(void *server)
{
	printf("In thread\n");
	int sockfd, numbytes;  
        char buf[MAXDATASIZE];
        struct addrinfo hints, *servinfo, *p;
        int rv;
        char s[INET6_ADDRSTRLEN];
	struct timespec start, end;

	memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

	printf("address: %s\n", ((subnet *)server)->ip);
        if ((rv = getaddrinfo(((subnet *)server)->ip, PORT, &hints, &servinfo)) != 0) {
                fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
                exit(1);
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
                exit(1);
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

	char * data = calloc(1024, sizeof(char));

	while(1)
	{
		//ping loop
		
		clock_gettime(CLOCK_REALTIME, &start);
		if (send(sockfd, data, 1024, 0) == -1)
			perror("send");


		if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
		{
			perror("recv");
		}
		clock_gettime(CLOCK_REALTIME, &end);
		buf[numbytes] = '\0';
        	
		double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / BILLION;
		RTT[((subnet *)server)->towerNum] = time_spent;
		//printf("client: ping '%s' from %s took %f ms\n\n",buf, s, time_spent * 1000);
		
		sleep(5);
	}

        close(sockfd);
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

int main()
{
	pthread_t tid[5];
	int i = 0;
	//char addresses[4][9] = {"10.0.1.1\0", "10.0.2.1\0", "10.0.3.1\0", "10.0.4.1\0"};
	subnet nets[4] = {
	{.towerNum = 0, .ip = "10.0.1.1\0"},
	{.towerNum = 1, .ip = "10.0.2.1\0"},
	{.towerNum = 2, .ip = "10.0.3.1\0"},
	{.towerNum = 3, .ip = "10.0.4.1\0"}};

	while(i < 4)
	{
		if(pthread_create(&tid[i], NULL, clientThread, (void *)&nets[i]) != 0)
			printf("Failed to create thread\n");
		i++;
		sleep(1);
	}
	
	//int time = 30;
	while(!(getc(stdin) == 'x'))
	{
		printf("\nCurrent ping values:\n0: %f ms\n1: %f ms\n2: %f ms\n3: %f ms\n\n", 
				1000 * RTT[0], 1000 * RTT[1], 1000 * RTT[2], 1000 * RTT[3]);

		int bestTower = 0;
		for (i = 1; i < 4; i++)
		{
			if (RTT[i] < RTT[bestTower])
				bestTower = i;
		}
		printf("The most optimal route is tower %d with %f ms RTT\n", bestTower, 1000 * RTT[bestTower]);

		
	}
	
	i = 0;
	while(i < 4)
	{
		pthread_cancel(tid[i++]);
		//pthread_join(tid[i++], NULL);
		printf("%d:\n", i);
	}

	return 0;
}
