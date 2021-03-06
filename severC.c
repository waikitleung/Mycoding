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

#define SEVERPORT "23620"
#define HOST "127.0.0.1"


void *get_in_addr(struct sockaddr *sa){      //get sockaddr IPV4 or IPV6
	if (sa->sa_family==AF_INET)
		{
			return &(((struct sockaddr_in*) sa)->sin_addr);
		}
		return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



int main(void){
	int UDPsocket;     // the socket used for received data and send result
	struct addrinfo hints, *servinfo, *p;
	int rv;
	struct sockaddr_storage their_addr;
	//char buf[MAXBUFLEN];
	socklen_t addr_len;
	float data;      // the data receive from aws


	memset(&hints,0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype =SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if((rv =getaddrinfo(HOST,SEVERPORT,&hints,&servinfo))!=0){  // return 0 if success
		fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(rv));
		return 1;   // means the process is not normal

	}

	for (p= servinfo;p!=NULL;p=p->ai_next){
		if ((UDPsocket= socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
			perror("serverC:socket");
			continue;

		}
		if (bind(UDPsocket,p->ai_addr,p->ai_addrlen)==-1){
			close(UDPsocket);
			perror("serverC:bind");
			continue;
		}
		break;

	}

	if(p == NULL){
		fprintf(stderr, "serverC: failed to bind the socket\n");
		return 1;
	}
	freeaddrinfo(servinfo);
	printf("The server C is up and using UDP on port %s.\n",SEVERPORT );
 	
 	addr_len=sizeof their_addr;
 	 	while(1)
 	{
 	recvfrom(UDPsocket,&data,sizeof data,0,(struct sockaddr*)&their_addr,&addr_len);
 	printf("The Server C reveived input <%f>.\n", data);


	float result3=0;

	result3=data*data*data*data*data;
	printf("The Server C calculated 5th power <%f>.\n", result3);


	// send back to  AWS
	sendto(UDPsocket,&result3,sizeof result3,0,(struct sockaddr *)&their_addr,  addr_len);
	 	printf("The Server C finished sending the output to AWS .\n");
}

	close(UDPsocket);
	return 0;
     /////////////
	}// main function
   