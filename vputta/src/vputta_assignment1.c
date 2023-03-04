/**
 * @vputta_assignment1
 * @author  venkata sai saran putta <vputta@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>

#include "../include/global.h"
#include "../include/logger.h"
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

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */


void createServer(char* portNumberStr);
void createClient(char* portNumberStr);



int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));

	/*Start Here*/

	if( argc != 3){

		printf("Enter correct parameters. client/server and port number are mandatory");
		exit(-1);
	}


	if( argv[1][0] == 's'){

       createServer(argv[2]);

	} else if( argv[1][0] == 'c'){

       createClient(argv[2]);

	} else {

		printf("Please check your parameters.");
		exit(-1);

	}


	return 0;

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

    char *ip = (char*) malloc(sizeof(char)*INET_ADDRSTRLEN);

    close(soc);



    return inet_ntop(AF_INET, &myaddress.sin_addr, ip, INET_ADDRSTRLEN);

}

char* getMyHostName(){

    char* hostbuffer = (char*) malloc(sizeof(char)*256);
    char * IPbuffer;
    struct hostent * host_entry;
    int hostname;
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));

    return hostbuffer;

}


int getMyPort(int socket){

    struct sockaddr_in sockaddress;
    socklen_t len=sizeof(sockaddress);


    if(getsockname(socket, (struct sockaddr *) & sockaddress, &len) == -1){
        return -1;
    }

    return ntohs(sockaddress.sin_port);

}

int credsValid(char* ip, char* port){

    //complete this function

    //ip should be valid. refer to the senior's code

    //port should be numeric && between 1 to 65535

    return 0;
}

