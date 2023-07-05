/*
	C socket server example
*/

#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write

int server(){
    char client_message[2000];
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    int n_connected_clients = 0;

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
    while(1){
        for(int i = 0; i < 2000; i++){
            client_message[i] = '\0';
        }
        listen(socket_desc , 3);

        //Accept and incoming connection
        puts("Waiting for incoming connections...");
        c = sizeof(struct sockaddr_in);

        //accept connection from an incoming client
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
        if (client_sock < 0)
        {
            perror("accept failed");
            return 1;
        }
        puts("Connection accepted");

        //Receive a message from client
        while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0  )
        {
            // Send the message back to the client
            write(client_sock, client_message, read_size);

            // Clean the client_message buffer
            memset(client_message, '\0', sizeof(client_message));
            // Send the message back to the client
        }


        if(read_size == 0)
        {
            puts("Client disconnected");
            fflush(stdout);
        }
        else if(read_size == -1)
        {
            perror("recv failed");
        }
    }
    //Listen

    return 0;
}

int main(int argc , char *argv[])
{
    server();
    return 0;
}

