#include <stdio.h>
#include <stdlib.h>
#include "../include/logger.h"
#include "../include/global.h"
#include "../include/commons.h"



#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <errno.h>

#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 256

int is_logged_in = 0;



void createClient(char* portNumberStr){

    int portNumber  =  atoi(portNumberStr);

    struct sockaddr_in client_addr;
    struct addrinfo hints, *res;

    /* Set up hints structure */
    memset(&hints, 0, sizeof(hints));
    	hints.ai_family = AF_INET;
    	hints.ai_socktype = SOCK_STREAM;
    	hints.ai_flags = AI_PASSIVE;

    /* Fill up address structures */
	if (getaddrinfo(NULL, portNumberStr, &hints, &res) != 0)
		perror("getaddrinfo failed");

    int client_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(client_socket < 0)
		perror("Cannot create socket");

    freeaddrinfo(res);




     if(bind(client_socket, res->ai_addr, res->ai_addrlen) != 0){

    	perror("Cannot bind socket");

    }

	while(TRUE){
		fflush(stdout);

		char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);
		memset(cmd, '\0', CMD_SIZE);
		if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
			exit(-1);


		cmd[strlen(cmd) - 1] = '\0';

        if(!strcmp("AUTHOR", cmd)){

            cse4589_print_and_log("[AUTHOR:SUCCESS]\n");
            cse4589_print_and_log("I, vputta, have read and understood the course academic integrity policy.\n");
            cse4589_print_and_log("[AUTHOR:END]\n");
        } else  if(!strcmp("IP", cmd)){

            char* ip = getMyIP();

            if(ip != NULL){

                cse4589_print_and_log("[IP:SUCCESS]\n");
                cse4589_print_and_log("IP:%s\n", ip);
            } else {
                cse4589_print_and_log("[IP:ERROR]\n");
            }
                cse4589_print_and_log("[IP:END]\n");

        } else  if(!strcmp("PORT", cmd)){

            int port = getMyPort(client_socket);

            if(port != -1){
                cse4589_print_and_log("[PORT:SUCCESS]\n");
                cse4589_print_and_log("PORT:%d\n", port);
            } else {
                cse4589_print_and_log("[PORT:ERROR]\n");
            }
                cse4589_print_and_log("[PORT:END]\n");


        } else if(!strcmp("LIST", cmd)) {

                cse4589_print_and_log("[LIST:SUCCESS]\n");



                cse4589_print_and_log("[LIST:END]\n");


        } else if(!strncmp("LOGIN", cmd,5)){

            char* login,* ip, *port;
            login = strtok(cmd, " ");
            ip = strtok(NULL, " ");
            port = strtok(NULL, " ");

            connect_to_host(client_socket,ip, port);

            struct message msg;
            char info[256];
            strcpy(msg.cmd,"LOGIN");
            strcpy(msg.ip,getMyIP());
            sprintf(info, "%s %d", getMyHostName(), getMyPort(client_socket));

            strcpy(msg.info, info);

            printf("%s", msg.info);
            send(client_socket, &msg, sizeof(msg), 0);



            is_logged_in = 1;


            fflush(stdout);





            //validations on ip and port


        } else if(!strcmp("REFRESH", cmd)){

        } else if(!strcmp("EXIT", cmd)){

        }

		/*if(send(server, msg, strlen(msg), 0) == strlen(msg))
			printf("Done!\n");
		fflush(stdout);*/

		/* Initialize buffer to receieve response */
		char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
		memset(buffer, '\0', BUFFER_SIZE);

		/*if(recv(server, buffer, BUFFER_SIZE, 0) >= 0){
			printf("Server responded: %s", buffer);
			fflush(stdout);
		}*/
	}
}

void connect_to_host(int client_socket, char *server_ip, char* server_port)
{

printf("%d %s %s", client_socket, server_ip, server_port);

	struct addrinfo hints, *res;

	/* Set up hints structure */
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/* Fill up address structures */
	if (getaddrinfo(server_ip, server_port, &hints, &res) != 0)
		perror("getaddrinfo failed");


	/* Connect */
	if(connect(client_socket, res->ai_addr, res->ai_addrlen) < 0)
		perror("Connect failed");

    printf("connection pass");

	freeaddrinfo(res);

}
