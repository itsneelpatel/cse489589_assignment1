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


struct connection{

    int client_port;
    char client_ip[64];
    char client_host_name[64];
    int fd_socket;

    struct connection *next;

} *connectionListHead = NULL;




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

                            cse4589_print_and_log("[LIST:SUCCESS]\n");

                            struct connection * temp = connectionListHead;

	                        int i = 1;

	                        while(temp){
	                        cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", i++, temp->client_host_name, temp->client_ip, temp->client_port);
	                        }

	                        cse4589_print_and_log("[LIST:END]\n");


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

                        char *ip = (char*) malloc(sizeof(char)*INET_ADDRSTRLEN);

                        inet_ntop(AF_INET, &client_addr.sin_addr, ip, INET_ADDRSTRLEN);

                        char hostName[256];
                        getnameinfo((struct sockaddr *)&client_addr, caddr_len,hostName, sizeof(hostName), 0,0,0);


						/* Add to watched socket list */
						FD_SET(fdaccept, &master_list);
						if(fdaccept > head_socket) head_socket = fdaccept;


						//create a connection node and add it to list

						struct connection * newCon = (struct connection *) malloc(sizeof (struct connection));
                            newCon->client_port = ntohs(client_addr.sin_port);
                            strcpy(newCon->client_ip, ip);
                            strcpy(newCon->client_host_name, hostName);
                            newCon->fd_socket= fdaccept;
                            newCon->next = NULL;

                        if(connectionListHead == NULL){

                            connectionListHead = newCon;

                        } else{
                            struct connection *temp = connectionListHead;

                            if( temp->client_port > newCon->client_port){

                                newCon->next = temp;
                                connectionListHead = newCon;

                            } else {
                                while( temp->next!= NULL && temp->next->client_port <  newCon->client_port ){

                                    temp = temp->next;


                                }

                                newCon->next = temp->next;
                                temp->next = newCon;
                            }


                        }

						/*strcpy(server_data.cmd,"LOGLISTOVER");
						if(send(fdaccept, &server_data, sizeof(server_data), 0) == sizeof(server_data))
	                            {

									printf("Done!\n");
	                            }*/
							fflush(stdout);





					}
					/* Read from existing clients */
					else{
						/* Initialize buffer to receieve response */
						struct message received;
						memset(&received, '\0', sizeof(received));

						if(recv(sock_index, &received, sizeof(received), 0) <= 0){
							close(sock_index);
							printf("Remote Host terminated connection!\n");

							/* Remove from watched list */
							FD_CLR(sock_index, &master_list);
						}
						else {
							//Process incoming data from existing clients here ...

							printf("\nClient sent me: %s\n", received.cmd);
							printf("ECHOing it back to the remote host ... ");
							//if(send(fdaccept, buffer, strlen(buffer), 0) == strlen(buffer))
								//printf("Done!\n");
							fflush(stdout);
						}

					}
				}
			}
		}
	}




}



