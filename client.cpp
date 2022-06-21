#include <bits/stdc++.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

using namespace std;

#define MAXLEN 2048 //Max allowed length of message

typedef struct sockaddr_in SA_IN; //IPv4
typedef struct sockaddr SA; //Protocol independent

int exflag = 0; //exit flag
int sockfd;
char name[32];

int check(int exp, const char *msg) {
    if(exp == -1){
        perror(msg) ;
        exit(1) ;
    }
    return exp ;
}

/* function to remove extra spaces after end of message */

void trm_str(char *s,int length){
	int i;
	for(i=0;i<length;i++){
		if(s[i]=='\n'){
			s[i]='\0';
			break;
		}
	}
}

/* function to handle the sending of messages from the client */

void* handler_send(void* arg) {
  char text[MAXLEN]={};
  char buf[MAXLEN + 32] = {};
  while(1) {
  
    fgets(text, MAXLEN, stdin);
    trm_str(text,MAXLEN);

    if (strcmp(text,"exit")==0) {
	break;
    } 
    else 
    {
        sprintf(buf,"%s: %s\n",name,text);
        send(sockfd, buf, strlen(buf), 0);
    }

    bzero(text, MAXLEN);
    bzero(buf, MAXLEN + 32);
  }
  exflag=1;
}

/* function to handle the sending of messages from the client */

void* handler_recieve(void* arg) {
    char text[MAXLEN]={};
    while (1) {
	int rec = recv(sockfd, text, MAXLEN, 0);
        if (rec > 0) {
            printf("%s", text);
            fflush(stdout);
        } 
        else if (rec == 0) {
	    break;
        } 
        else {
	}
	memset(text, 0, sizeof(text));
    }
}

int main(int argc, char **argv){
	
	cout<<" Enter your name... ";
        fgets(name, 32, stdin);

	if (strlen(name) > 32 || strlen(name) < 2){
		printf("Invalid length for the name.\n");
		return EXIT_FAILURE;
	}
	SA_IN server_addr;

	/* Socket settings */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
        server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(10000);
	server_addr.sin_addr.s_addr=INADDR_ANY;

    int cli_con=connect(sockfd,(SA*)&server_addr,sizeof(server_addr));

	send(sockfd, name, 32, 0);

	cout<<"\n\t||-----Entering the Chat Room-----||\n";

	pthread_t send_thread,recieve_thread;
    
    if(pthread_create(&send_thread, NULL, &handler_send, NULL) != 0){
		printf("ERROR: pthread\n");
        return EXIT_FAILURE;
	}

    if(pthread_create(&recieve_thread, NULL, &handler_recieve, NULL) != 0){
		printf("ERROR: pthread\n");
        return EXIT_FAILURE;
	}

	while (1){
		if(exflag){
			cout<<"\n!leaving\n";
			break;
        	}
	}
	close(sockfd);
	return EXIT_SUCCESS;
}
