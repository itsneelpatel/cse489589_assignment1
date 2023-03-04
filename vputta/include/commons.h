#ifndef COMMONS_H_
#define COMMONS_H_

extern char* getMyIP();
extern int getMyPort(int socket);
extern char* getMyHostName();
extern int credsValid(char* ip, char* port);

struct message{
    char cmd[10];
    char ip[32];
    char info[256];
};
#endif

