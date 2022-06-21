//libraries
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

#define BACKLOG 8 //Max number of clients pending to join
#define MAXCLIENTS 100 //Max number of clients allowed in the server
#define BUFFERLEN 2048 //Allowed length of the longest message sent by the client

static unsigned int Nofclient = 0; //No of clients in the server
static int userId = 10;

typedef struct sockaddr_in SA_IN; //IPv4
typedef struct sockaddr SA; //Protocol Independent


string reset_col = "\033[0m";

typedef struct{
	SA_IN address;
	int sockfd;
	int userId;
	char name[32];
} client_threads;

client_threads *users[MAXCLIENTS]; // List of clients

pthread_mutex_t cli_mtx = PTHREAD_MUTEX_INITIALIZER;

int check(int exp, const char *msg) {
    if(exp == -1){
        perror(msg) ;
        exit(1) ;
    }
    return exp ;
}

/* function to add a new client to queue */

void add2queue(client_threads *clie){
	pthread_mutex_lock(&cli_mtx);
	for(int i=0; i < MAXCLIENTS; ++i){
		if(!users[i]){
			users[i] = clie;
			break;
		}
	}
	pthread_mutex_unlock(&cli_mtx);
}

/* function to remove a client from queue of users */

void remFromQueue(int id){
	pthread_mutex_lock(&cli_mtx);
	for(int i=0; i < MAXCLIENTS; ++i){
		if(users[i]){
			if(users[i]->userId == id){
				users[i] = NULL;
				break;
			}
		}
	}
	pthread_mutex_unlock(&cli_mtx);
}

/* function to broadcast message form a client to all other clients as well as the server */

void broadcast_message(char *s, int id){
	pthread_mutex_lock(&cli_mtx);

	for(int i=0; i<MAXCLIENTS; ++i){
		if(users[i]){
			if(users[i]->userId != id){
				if(write(users[i]->sockfd, s, strlen(s)) < 0){
					perror("ERROR: Failed to write to user");
					break;
				}
			}
		}
	}
	pthread_mutex_unlock(&cli_mtx);
}

/* function to handle all interactions to the client */

void *handle_client(void *arg){
	char buf_out[BUFFERLEN];
	char name[32];
	bool ex = false; //exit flag

	Nofclient++;
	client_threads *clie = (client_threads *)arg;

	if(recv(clie->sockfd, name, 32, 0) < 1 || strlen(name) <  2 || strlen(name) > 32){
		cout<<" No name entered ";
		ex = true;
	} 
	else{
		strcpy(clie->name, name);
		sprintf(buf_out,"\033[36m%s has joined the room\033[0m \n",clie->name);
		printf("%s", buf_out);
		broadcast_message(buf_out, clie->userId);
		cout<<reset_col;
	}
	bzero(buf_out, BUFFERLEN); //to clear out buf_out array

	while(1){
		if (ex) {
			break;
		}
        	
        	int mes_rec = recv(clie->sockfd, buf_out, BUFFERLEN, 0);
		if (mes_rec > 0){
			if(strlen(buf_out) > 0){
				broadcast_message(buf_out, clie->userId);
				printf("%s", buf_out);
			}
		} 
		else if (mes_rec == 0 || strcmp(buf_out, "exit") == 0){
			cout<<"\033[91m";
			sprintf(buf_out,"%s\033[91m has left the room\033[0m\n", clie->name);
			cout<<reset_col;
			printf("%s", buf_out);
			broadcast_message(buf_out, clie->userId);
			ex = true;
		} 
		else {
			printf("ERROR: -1\n");
			ex = true;
		}

		bzero(buf_out, BUFFERLEN);
	}

  /* Delete client from queue and yield thread */
        close(clie->sockfd);
  	remFromQueue(clie->userId);
  	Nofclient--;
	free(clie);
  	pthread_detach(pthread_self());

	return NULL;
}

int main(int argc, char **argv){
	int server_socket,client_socket;
	SA_IN client_addr,server_addr;
	pthread_t tid;
	check(server_socket=socket(AF_INET,SOCK_STREAM,0),
	"Failed to create socket");

	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(10000);
	server_addr.sin_addr.s_addr=INADDR_ANY;
	bzero(&server_addr.sin_zero,0);
	
	check(bind(server_socket,(SA*)&server_addr,sizeof(server_addr)),"Failed to Bind the Socket");
	check(listen(server_socket,8),"Failed to listen to socket");

    cout<<"\n\t||-----Opening the Chat Room-----||\n";

    while(true){
		socklen_t clen = sizeof(client_addr);
		int c_acpt = accept(server_socket, (SA*)&client_addr, &clen);

		if(Nofclient == MAXCLIENTS-1){
			cout<<"Maximum number of users has been reached. Request to join room is rejected";
			close(c_acpt);

		}
		client_threads *curcli = (client_threads*)malloc(sizeof(client_threads));
		curcli->address = client_addr;
		curcli->sockfd = c_acpt;
		curcli->userId = userId++;

		add2queue(curcli);
		pthread_create(&tid, NULL, &handle_client, (void*)curcli);

		sleep(1);
    }
	exit(0);
}
