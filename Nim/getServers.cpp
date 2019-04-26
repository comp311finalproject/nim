#include "nim.h"
#include <WinSock2.h>
#include <iostream>

// getServers.cpp
//  Given a socket, a broadcast address and a port number, this function returns the number
//  of TicTacToe servers as well as an array of structs.  Each struct contains the name, IP_Address and 
//  port number of a remote NIM server.

//    IN parameters:
//    SOCKET s                = Allocated UDP socket handle that can be used for communcations
//    char *broadcastAddress  = The broadcast address that should be used for the current LAN
//    char *broadcastPort     = The port that should be used for the broadcast

//    OUT parameters:
//    ServerStruct serverArray[]  = An array that contains the structs.  The members of each struct
//                              contain the name, IP Address and port number of a remote NIM server.
//    numServers              = Number of elements in serverArray[]

//    Function return value   = Number of remote NIM servers (size of serverArray[])

int getServers(SOCKET s, const char *broadcastAddress, const char *remotePort, ServerStruct serverArray[])
{
	// This function returns the number of NIM servers that may be found within the current Broadcast Domain.
	// The last parameter, serverArray[], will contain all of the servers' names, IP addresses and port numbers.

	int numServers = 0;

	// Send the constant, NIM_QUERY, to the broadcastAddress using remotePort
	UDP_send(s, NIM_QUERY, MAX_SEND_BUFFER, broadcastAddress, remotePort);

	// Receive incoming UDP datagrams (with a maximum of 2 second wait before each UDP_recv() function call
	// As you read datagrams, if they start with the prefix: NIM_NAME, parse out the server's name
	// and add the name, host address and port number to serverArray[].  Don't forget to increment numServers.
	int status = wait(s, 2, 0);
	if (status > 0) {
		int numBytesRecvd = 0;
		char recvBuffer[MAX_RECV_BUFFER + 1];
		char host[v4AddressSize];
		char port[portNumberSize];
	
		numBytesRecvd = UDP_recv(s, recvBuffer, MAX_RECV_BUFFER, host, port);

		// Ignoring responses that were sent using the broadcastAddress.  We need specific IP Address
		while (status > 0 && numBytesRecvd > 0 && strcmp(host, broadcastAddress) != 0) {
			
			char validation[6] = "Name=";
			if (strstr(recvBuffer, validation) != 0)
			{
				serverArray[numServers].name = recvBuffer + 5;
				serverArray[numServers].host = host;
				serverArray[numServers].port = port;
				numServers++;
			}
			// Now, we'll see if there is another response.
			status = wait(s, 2, 0);
			if (status > 0) {
				numBytesRecvd = UDP_recv(s, recvBuffer, MAX_RECV_BUFFER, host, port);
				if (debug) {
					std::cout << "Received: " << recvBuffer << " from " << host << ":" << port << std::endl;
				}
			}
		}
	}
	return numServers;
}
