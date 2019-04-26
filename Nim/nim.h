#pragma once
// nim.h    Header file for Nim project
//<<<<<<< HEAD
//=======

#include <winsock2.h>
#include <string>

#define NIM_QUERY	"Who?"
#define NIM_NAME	"Name="
#define NIM_CHALLENGE	"Player="
#define NIM_CONFIRMATION "Great!"
#define debug true

static char NIM_UDPPORT[] = "29333";

const int v4AddressSize = 16;
const int portNumberSize = 10;
const int WAIT_TIME = 30;
const int MAX_SEND_BUFFER = 2048;
const int MAX_RECV_BUFFER = 2048;
const int MAX_SERVERS = 100;
const int MAX_NAME = 100;
const int SERVER_PLAYER = 2;
const int CLIENT_PLAYER = 1;
const int MAX_NIM_BOARD_SIZE = 20;
const int MIN_NIM_BOARD_SIZE = 3;
const int MAX_NIM_PILE_SIZE = 20;
const int MIN_NIM_PILE_SIZE = 1;
const int MAX_NIM_MOVE_SIZE = 3;
const int ABORT = -1;

struct ServerStruct {
	std::string name;
	std::string host;
	std::string port;
};

int playNim(SOCKET, std::string, std::string, int);
SOCKET connectsock(const char*, const char*, const char*);
SOCKET passivesock(const char*, const char*);
int UDP_recv(SOCKET, char*, int, char*, char*);
int UDP_send(SOCKET, const char*, int, const char*, const char*);
int wait(SOCKET, int, int);
char* timestamp();
int getServers(SOCKET, const char*, const char*, ServerStruct[]);
int serverMain(std::string);
int clientMain(std::string);
int getIPAddressInfo(char*, char*);