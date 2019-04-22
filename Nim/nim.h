#pragma once
// nim.h    Header file for Nim project
// Sam Dillin
//Adam Johnson
#include <winsock2.h>
#include <string>

#define NIM_QUERY	"Who?"
#define NIM_NAME	"Name="
#define NIM_CHALLENGE	"Player="

static char NIM_UDPPORT[] = "29333";

const int v4AddressSize = 16;
const int portNumberSize = 10;
const int WAIT_TIME = 120;
const int MAX_SEND_BUFFER = 2048;
const int MAX_RECV_BUFFER = 2048;
const int MAX_SERVERS = 100;
const int MAX_NAME = 100;

struct ServerStruct {
	std::string name;
	std::string host;
	std::string port;
};

void playNim(SOCKET, bool, char*, bool, bool, char*, char*, bool);
SOCKET connectsock(const char*, const char*, const char*);
SOCKET passivesock(const char*, const char*);
int UDP_recv(SOCKET, char*, int, char*, char*);
int UDP_send(SOCKET, const char*, int, const char*, const char*);
int wait(SOCKET, int, int);
char* timestamp();
int getServers(SOCKET, char*, char*, ServerStruct[]);
int serverMain(int, char*);
int clientMain(int, char*);
int getIPAddressInfo(char*, char*);