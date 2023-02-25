#include <stdio.h>
#include <stdlib.h>
#include "../include/logger.h"
#include "../include/global.h"


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


int getMyPort(int socket){

    struct sockaddr_in sockaddress;
    socklen_t len=sizeof(sockaddress);


    if(getsockname(socket, (struct sockaddr *) & sockaddress, &len) == -1){
        return -1;
    }

    return ntohs(sockaddress.sin_port);

}

char* getMyIP(){


    const char* googleDnsServerIp = "8.8.8.8";
    const int dnsPort = 53;

    int soc = socket(AF_INET, SOCK_DGRAM, 0);

    if(soc == -1){
        return NULL;
    }



    struct sockaddr_in googleaddress, myaddress;

    memset(&googleaddress, 0 , sizeof(googleaddress));

    googleaddress.sin_family = AF_INET;
    googleaddress.sin_addr.s_addr = inet_addr( googleDnsServerIp );
    googleaddress.sin_port = htons( dnsPort );

    int connectStatus = connect( soc , (struct sockaddr*) &googleaddress , sizeof(googleaddress) );

    if(connectStatus == -1){
        return NULL;
    }

    socklen_t len = sizeof(myaddress);

    if(getsockname(soc, (struct sockaddr *) & myaddress, &len) == -1){
        return NULL;
    }

    char *strbuf = (char*) malloc(sizeof(char)*100);;

    close(soc);


    return inet_ntop(AF_INET, &myaddress.sin_addr, strbuf, 100);

}

void createServer(char* portNumberStr){

    int portNumber  =  atoi(portNumberStr);

    int server_socket, head_socket, selret, sock_index, fdaccept=0, caddr_len;
	struct sockaddr_in client_addr;
	struct addrinfo hints, *res;
	fd_set master_list, watch_list;

	/* Set up hints structure */
	memset(&hints, 0, sizeof(hints));
    	hints.ai_family = AF_INET;
    	hints.ai_socktype = SOCK_STREAM;
    	hints.ai_flags = AI_PASSIVE;

	/* Fill up address structures */
	if (getaddrinfo(NULL, portNumberStr, &hints, &res) != 0)
		perror("getaddrinfo failed");

	/* Socket */
	server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(server_socket < 0)
		perror("Cannot create socket");

	/* Bind */
	if(bind(server_socket, res->ai_addr, res->ai_addrlen) < 0 )
		perror("Bind failed");

	freeaddrinfo(res);

	/* Listen */
	if(listen(server_socket, BACKLOG) < 0)
		perror("Unable to listen on port");

	/* ---------------------------------------------------------------------------- */

	/* Zero select FD sets */
	FD_ZERO(&master_list);
	FD_ZERO(&watch_list);

	/* Register the listening socket */
	FD_SET(server_socket, &master_list);
	/* Register STDIN */
	FD_SET(STDIN, &master_list);

	head_socket = server_socket;

	while(TRUE){
		memcpy(&watch_list, &master_list, sizeof(master_list));

		//printf("\n[PA1-Server@CSE489/589]$ ");
		//fflush(stdout);

		/* select() system call. This will BLOCK */
		selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
		if(selret < 0)
			perror("select failed.");

		/* Check if we have sockets/STDIN to process */
		if(selret > 0){
			/* Loop through socket descriptors to check which ones are ready */
			for(sock_index=0; sock_index<=head_socket; sock_index+=1){

				if(FD_ISSET(sock_index, &watch_list)){

					/* Check if new command on STDIN */
					if (sock_index == STDIN){
						char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

						memset(cmd, '\0', CMD_SIZE);
						if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
							exit(-1);


                        cmd[strlen(cmd) - 1] = '\0';

                        if(!strcmp("AUTHOR", cmd)){

                            cse4589_print_and_log("[AUTHOR:SUCCESS]\n");
                            cse4589_print_and_log("I, vputta, have read and understood the course academic integrity policy.\n");
                            cse4589_print_and_log("[AUTHOR:END]\n");

                        } else if(!strcmp("IP", cmd)){
                            char* ip = getMyIP();

                            if(ip != NULL){

                                cse4589_print_and_log("[IP:SUCCESS]\n");
                                cse4589_print_and_log("IP:%s\n", ip);
                            } else {
                                cse4589_print_and_log("[IP:ERROR]\n");
                            }
                            cse4589_print_and_log("[IP:END]\n");


                        } else if(!strcmp("PORT", cmd)){
                            int port = getMyPort(server_socket);

                            if(port != -1){
                                cse4589_print_and_log("[PORT:SUCCESS]\n");
                                cse4589_print_and_log("PORT:%d\n", port);
                            } else {
                                cse4589_print_and_log("[PORT:ERROR]\n");
                            }
                            cse4589_print_and_log("[PORT:END]\n");


                        } else if(!strcmp("LIST", cmd)) {



                        }



						//Process PA1 commands here ...

						free(cmd);
					}
					/* Check if new client is requesting connection */
					else if(sock_index == server_socket){
						caddr_len = sizeof(client_addr);
						fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
						if(fdaccept < 0)
							perror("Accept failed.");

						printf("\nRemote Host connected!\n");

						/* Add to watched socket list */
						FD_SET(fdaccept, &master_list);
						if(fdaccept > head_socket) head_socket = fdaccept;
					}
					/* Read from existing clients */
					else{
						/* Initialize buffer to receieve response */
						char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
						memset(buffer, '\0', BUFFER_SIZE);

						if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
							close(sock_index);
							printf("Remote Host terminated connection!\n");

							/* Remove from watched list */
							FD_CLR(sock_index, &master_list);
						}
						else {
							//Process incoming data from existing clients here ...

							printf("\nClient sent me: %s\n", buffer);
							printf("ECHOing it back to the remote host ... ");
							if(send(fdaccept, buffer, strlen(buffer), 0) == strlen(buffer))
								printf("Done!\n");
							fflush(stdout);
						}

						free(buffer);
					}
				}
			}
		}
	}




}



