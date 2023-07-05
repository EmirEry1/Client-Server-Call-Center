/*
	C socket server example
*/

#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include <pthread.h>
#include <sys/time.h>
#include <malloc.h>

int n_connections = 0;
pthread_mutex_t connections;

typedef struct {
    int socket;
    struct sockaddr_in address;
    int user_no;
} Client;

Client *clients[3];

void broadcast(char client_message[2000], int sender_no){
    for(int i=0; i<3;i++){
        if(clients[i]) {
            if (clients[i]->socket != 0 && clients[i]->user_no == sender_no) {
                write(clients[i]->socket, client_message, strlen(client_message));
            }
            else if(clients[i]->user_no == sender_no){
                //write(clients[i]->socket, "", strlen(client_message));
            }
        }
    }
}

void *handle_connection(Client* client){
//    int client_sock = client->socket;
    int read_size;
    int user_no = client->user_no;
    char client_message[2000];

    if (n_connections >= 3) {
        write(client->socket, "Unfortunately, we can't accept you due to server being busy. Please try again later.",
              2000);
        close(client->socket);
    } else {

        if (client->socket < 0) {
            perror("accept failed");
        }
        else {
//            puts("Connection accepted\n");

            pthread_mutex_lock(&connections);
            n_connections++; //Possible racing condition
            pthread_mutex_unlock(&connections);
            if (n_connections > 2) {
                write(client->socket, "The chatroom is full. You will be let in when it is empty.", 2000);
                while (n_connections > 2);
                pthread_mutex_lock(&connections);
                if(!clients[0]){
                    client->user_no = 1;
                    clients[0] = client;
                }
                else if(!clients[1]){
                    client->user_no = 2;
                    clients[1] = client;
                }
                pthread_mutex_unlock(&connections);
            }

            write(client->socket, "Welcome. You can send your message! You will have 10 seconds starting from now.",
                  2000);
            struct timeval start_time, end_time;
            gettimeofday(&start_time, NULL);
            gettimeofday(&end_time, NULL);
            while ((read_size = recv(client->socket, client_message, 2000, 0)) > 0 &&(double) (end_time.tv_sec - start_time.tv_sec +(end_time.tv_usec - start_time.tv_usec) / 1000000.0) < 10) {
                //Send the message back to client
//                broadcast(client_message,client->user_no);
                gettimeofday(&end_time, NULL);
                if((double) (end_time.tv_sec - start_time.tv_sec +(end_time.tv_usec - start_time.tv_usec) / 1000000.0) > 10){
                    memset(client_message, '\0', sizeof(client_message));
                    break;
                }
                write(client->socket, client_message, strlen(client_message));
                memset(client_message, '\0', sizeof(client_message));
                gettimeofday(&end_time, NULL);
            }
            if (read_size == 0) {
                pthread_mutex_lock(&connections);
                if(close(client->socket) == 0){

                    puts("Client disconnected");
                    fflush(stdout);
                    for(int i = 0; i < 3; i++){
                        if(clients[i]){
                            if(clients[i]->user_no == user_no){
                                clients[i] = NULL;
                                free(client);
                            }
                        }
                    }
                    n_connections--;

                }
                else {
                    perror("Error closing connection");
                }
                pthread_mutex_unlock(&connections);

            } else if (read_size == -1) {
                perror("recv failed");
            }
            else{
                pthread_mutex_lock(&connections);
                write(client->socket, "Your time is up. Goodbye", 2000);
                if (close(client->socket) == 0) {
                    printf("Connection closed successfully.\n");
                    for(int i = 0; i < 3; i++){
                        if(clients[i]) {
                            if (clients[i]->user_no == user_no) {
                                clients[i] = NULL;
                                free(client);
                            }
                        }
                    }
                    n_connections--;
                } else {
                    perror("Error closing connection");
                }
                pthread_mutex_unlock(&connections);
                //Receive a message from client
            }

        }
    }

}


int main(int argc , char *argv[])
{
    //pthread_t handle_clients[3];
    pthread_mutex_init(&connections, NULL);
    int socket_desc;
    struct sockaddr_in server;


    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );

    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");

    //Listen

    listen(socket_desc , 3);
    int read_size;
    char client_message[2000];
    struct sockaddr_in client;
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    int c = sizeof(struct sockaddr_in);
    int client_socket;
    pthread_t sniffer_thread;

    while( 1 )
    {
        client_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_socket<0)
        {
            perror("accept failed");
            return 1;
        }

        puts("Connection accepted");
        Client *cli = (Client *)malloc(sizeof(Client));
        cli->address = client;
        cli->socket = client_socket;
        if(!clients[0]){
            cli->user_no = 1;
            clients[0] = cli;
        }
        else if(!clients[1]){
            cli->user_no = 2;
            clients[1] = cli;
        }

        if( pthread_create( &sniffer_thread , NULL ,  &handle_connection , cli ) < 0)
        {
            perror("could not create thread");
            return 1;
        }

        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("Handler assigned");
    }
    return 0;
}

