/*------------------------------------------------------------
 * UDP_recv - Receive a string from a remote machine using UDP
 *------------------------------------------------------------*/
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

#define MAX_IP_ADDRESS_LEN 16
#define MAX_PORT_LEN 6

int UDP_recv(SOCKET s, char *buf, int maxSize, char *remoteHost, char *remotePort)
{	// Parameter List:
	// [IN] s            : SOCKET handle
	// [OUT] buf         : Address of NULL-terminated char array that will contain the data received.
	// [IN] maxSize      : Integer containing the maximum number of bytes that buf can hold
	// [OUT] remoteHost  : Address of NULL-terminated char array containing IP Address of machine that sent the data
	// [OUT] remotePort  : Address of NULL-terminated char array containing port number on remote host that sent the data

	// Return Value: Number of bytes actually received.

	// NOTES: 1. This function requires WinSock2.h and iostream header files
	//             as well as the ws2_32.lib library file.
	//        2. This function will block until data is received from some other machine.

	sockaddr_in sin;						/* an Intenet endpoint address */
	int         sin_size = sizeof(sin);		/* Needed for sixth parameter in recvfrom function call */
	int         numBytes;					/* Number of bytes actually received */

	if (s < 0 || s == INVALID_SOCKET) {
		std::cout << "Problem with UDP_recv().  Invalid socket handle." << std::endl;
		numBytes = -1;
	}
	else {
		memset(&sin, 0, sizeof(sin));				/* zero the sockaddr_in struct */

		/**** BLOCKING function ****
		/* Wait for and read incoming data from ANY remote host/port */
		numBytes = recvfrom(s, buf, maxSize, 0, (LPSOCKADDR)&sin, &sin_size);
		if (numBytes <= 0) {
			std::cout << std::endl << "Problem with recvfrom() function call within UDP_recv().  Error Code = " << WSAGetLastError() << std::endl;
		}
		else if (numBytes > maxSize) {
			std::cout << std::endl << "Problem with recvfrom() function call within UDP_recv().  Received data exceeded buffer size" << std::endl;
		}
		else {
			// Find the remote sender's IP address and port #
			char host[MAX_IP_ADDRESS_LEN];
			if (inet_ntop(AF_INET, &sin.sin_addr, host, sizeof(host)) == NULL) {
				std::cout << "Problem with inet_ntop() function in UDP_recv()." << std::endl;
			}
			strcpy_s(remoteHost, 16, host);

			_itoa_s(ntohs(sin.sin_port), remotePort, MAX_PORT_LEN, 10);

			// Make sure the characters in buf[] are null-terminated
			if (buf[numBytes - 1] != '\0') {
				if (numBytes < maxSize)
					buf[numBytes] = '\0';
				else
					buf[maxSize - 1] = '\0';
			}
		}
	}
	return numBytes;
}