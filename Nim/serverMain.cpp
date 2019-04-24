// server_main.cpp
//   This function serves as the "main" function for the server-side
#include "nim.h"
#include <iostream>
#include <string>
#include <WinSock2.h>

int serverMain(int argc, char *argv[], std::string playerName)
{
	SOCKET s;
	char buffer[MAX_RECV_BUFFER];
	char host[v4AddressSize];
	char port[portNumberSize];
	char responseStr[MAX_SEND_BUFFER];
	int winner;

	s = passivesock(NIM_UDPPORT, "udp");

	std::cout << std::endl << "Waiting for a challenge..." << std::endl;
	int len = UDP_recv(s, buffer, MAX_RECV_BUFFER, host, port);
	if (debug) {
		std::cout << timestamp() << " - Received: " << buffer << " from " << host << ":" << port << std::endl;
	}

	bool finished = false;
	while (!finished) {
		if (strcmp(buffer, NIM_QUERY) == 0) {
			// Respond to name query
			strcpy_s(responseStr, NIM_NAME);
			strcat_s(responseStr, playerName.c_str());
			if (debug) {
				std::cout << timestamp() << " - Sending: " << responseStr << " to " << host << ":" << port << std::endl;
			}
			UDP_send(s, responseStr, strlen(responseStr) + 1, host, port);

		}
		else if (strncmp(buffer, NIM_CHALLENGE, strlen(NIM_CHALLENGE)) == 0) {
			// Received a challenge  
			char *startOfName = strstr(buffer, NIM_CHALLENGE);
			if (startOfName != NULL) {
				std::cout << std::endl << "You have been challenged by " << startOfName + strlen(NIM_CHALLENGE) << std::endl;
			}

			// Give option for user to decline
			char sendBuffer[MAX_SEND_BUFFER];
			std::cout << "Would you like to accept the challenge from " << startOfName + strlen(NIM_CHALLENGE) << "? YES or NO" << std::endl;
			std::cin >> sendBuffer;
			UDP_send(s, sendBuffer, MAX_SEND_BUFFER, host, port);
			if (strcmp(sendBuffer, "YES")) {
				wait(s, 2, 0);
				UDP_recv(s, responseStr, MAX_RECV_BUFFER, host, port);
				if (strcmp(responseStr, NIM_CONFIRMATION)) {
					winner = playNim();//FIGURE OUT PARAMETERS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					finished = true;
				}
				else
				{
					std::cout << "Connection error. Waiting for another challenge.." << std::endl;
				}
			}
			else {
				std::cout << "Waiting for another challenge..." << std::endl;
			}
		}

		if (!finished) {
			char previousBuffer[MAX_RECV_BUFFER];	strcpy_s(previousBuffer, buffer);
			char previousHost[v4AddressSize];				strcpy_s(previousHost, host);
			char previousPort[portNumberSize];				strcpy_s(previousPort, port);

			// Check for duplicate datagrams (can happen if broadcast enters from multiple ethernet connections)
			bool newDatagram = false;
			int status = wait(s, 1, 0);	// We'll wait a second to see if we receive another datagram
			while (!newDatagram && status > 0) {
				len = UDP_recv(s, buffer, MAX_RECV_BUFFER, host, port);
				if (debug) {
					std::cout << timestamp() << " - Received: " << buffer << " from " << host << ":" << port << std::endl;
				}
				if (strcmp(buffer, previousBuffer) == 0 &&		// If this datagram is identical to previous one
					strcmp(host, previousHost) == 0 &&		//  and it came from a previously known host
					strcmp(port, previousPort) == 0) {		//  using the same port number, then ignore it.
					status = wait(s, 1, 0);			// Wait another second (still more copies?)
				}
				else {
					newDatagram = true;		// if not identical to previous one, keep it!
				}
			}

			// If we waited one (or more seconds) and received no new datagrams, wait for one now.
			if (!newDatagram) {
				len = UDP_recv(s, buffer, MAX_RECV_BUFFER, host, port);
				if (debug) {
					std::cout << timestamp() << " - Received: " << buffer << " from " << host << ":" << port << std::endl;
				}
			}
		}
	}
	closesocket(s);

	return 0;
}