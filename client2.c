/*
	C ECHO client example using sockets
*/
#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>

char time_up_message[2000] = "Your time is up. Goodbye";
char welcome_message[2000] = "Welcome. You can send your message! You will have 10 seconds starting from now.";
char server_busy_message[2000] = "Unfortunately, we can't accept you due to server being busy. Please try again later.";
char full_chatroom_message[2000] = "The chatroom is full. You will be let in when it is empty.";


int main(int argc , char *argv[])
{
    int sock;
    struct sockaddr_in server;
    char message[1000] , server_reply[2000];

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    recv(sock, server_reply, 2000, 0);

    if(server_reply!=NULL && server_reply[0] =='U'){
        puts(server_reply);
        close(sock);
        return 0;
    }
    if(!strcmp (server_reply, full_chatroom_message)) {
        puts(server_reply);
        while (!strcmp (server_reply, full_chatroom_message)) {
            memset(server_reply, '\0', sizeof(server_reply));
            recv(sock, server_reply, 2000, 0);
            puts(server_reply);
        }
    }
    else{
        puts(server_reply);
    }
    memset(server_reply, '\0', sizeof(server_reply));
    //keep communicating with server
    while(1)
    {
        printf("Enter message : ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';
//        fgets("%s" , 2000, message);
        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
        //Receive a reply from the server
        if( recv(sock , server_reply , 2000 , 0) < 0)
        {
            puts("recv failed");
            break;
        }

        puts("Server reply :");
        puts(server_reply);
//        if(server_reply[0] == 'Y'){ //TODO: This is only for experimental purposes. Server can respond with Y normally too.
//            close(sock);
//            return 0;
//        }
        if(!strcmp (server_reply, time_up_message)){ //TODO: This is only for experimental purposes. Server can respond with Y normally too.
            close(sock);
            return 0;
        }
        memset(server_reply, '\0', sizeof(server_reply));
    }

    close(sock);
    return 0;
}

