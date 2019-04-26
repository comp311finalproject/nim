/*------------------------------------------------------------------
 * connectsock - allocate & connect a client socket using TCP or UDP
 *------------------------------------------------------------------*/
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include "nim.h"

SOCKET connectsock(const char *host, const char *service, const char *protocol) {
	// Parameter List:
	// [IN] host    : String containing host as either a DNS name (eg. "www.harding.edu") or an IP address (e.g. "162.242.214.217")
	// [IN] service : String containing service type as name (eg. "ECHO") or port number (eg. "7")
	// [IN] protocol: String containing protocol type (either "tcp" or "udp")
	// Return value : SOCKET handle, s, of connected socket.

	//	NOTES: 1. This function requires the iostream and winsock2.h header files as well as the ws2_32.lib library file.
	//         2. The host and service parameters are not used for UDP sockets.  
	//				Empty strings may be used for those two parameters if protocol = "UDP".

	SOCKET			s;				// socket handle                        
	struct sockaddr_in clientInfo;	// an Intenet endpoint address  (socket information)        
	struct servent  *pse;			// pointer to service information entry  

	// Allocate and connect socket - type depends upon protocol 
	// If using TCP ...
	if (_stricmp(protocol, "tcp") == 0) {
		/* Create a TCP socket */
		s = socket(AF_INET, SOCK_STREAM, 0);
		if (s < 0 || s == INVALID_SOCKET) {
			std::cout << "Cannot create socket.  Error Code = " << WSAGetLastError() << std::endl;
			return INVALID_SOCKET;
		}

		memset(&clientInfo, 0, sizeof(clientInfo));		// zero the sockaddr_in struct

		// Now populate clientInfo using three steps: A, B, and C:

		// A. Set the address family to AF_INET (Address Family is Internet) 
		clientInfo.sin_family = AF_INET;

		// B. Map host name to remote IP address, allowing for possibility of dotted decimal notation
		// 1st try to convert from "dotted decimal notation" to 32-bit address in network byte order (big endian)
		int result = inet_pton(AF_INET, host, &(clientInfo.sin_addr));	// If result == 1, this worked!
		if (result <= 0) {
			// If that doesn't work, try sending a request to DNS using the name of the remote host
			struct addrinfo *result = NULL;
			DWORD dwRetval = getaddrinfo(host, service, 0, &result);
			if (dwRetval == 0) {	// If DNS responded with valid response (ie. dwRetval is 0),  
									//    Copy the returned 32-bit address to clientInfo
				struct sockaddr_in  *sockaddr_ipv4;
				sockaddr_ipv4 = (struct sockaddr_in *) result->ai_addr;
				clientInfo.sin_addr.s_addr = sockaddr_ipv4->sin_addr.S_un.S_addr;
			}
			else {
				if (dwRetval == WSAHOST_NOT_FOUND) {
					std::cout << "Remote host not found" << std::endl;
				}
				else {
					std::cout << "getaddrinfo failed with error = " << dwRetval << std::endl;
				}
				return SOCKET_ERROR;
			}
		}

		// C. Map service name (or number) to port number field
		pse = getservbyname(service, protocol);	// Search for matching service name
		if (pse != NULL) {						// If found, retrieve associated port number
			clientInfo.sin_port = (u_short)pse->s_port;
		}
		else {									// If not found, treat service parameter as an integer
			short port = atoi(service);
			if (port > 0) {
				clientInfo.sin_port = htons(port);
			}
			else {
				// That didn't work either, punt.
				std::cout << "Invalid service request" << std::endl;
				return INVALID_SOCKET;
			}
		}

		// Now ready to try to connect to the remote TCP socket
		int status = connect(s, (LPSOCKADDR)&clientInfo, sizeof(SOCKADDR));
		if (status == SOCKET_ERROR) {
			std::cout << "Remote host/service not found - or connection refused.  Error Code = " << WSAGetLastError() << std::endl;
			return INVALID_SOCKET;
		}
	}

	// If using UDP ...
	else if (_stricmp(protocol, "udp") == 0) {
		// Create a UDP socket (Nothing else to do!!)
		s = socket(AF_INET, SOCK_DGRAM, 0);
		if (s < 0 || s == INVALID_SOCKET) {
			std::cout << "Cannot create socket.  Error Code = " << WSAGetLastError() << std::endl;
			return INVALID_SOCKET;
		}
	}

	// This function doesn't support anything other than TCP or UDP
	else {
		std::cout << "Invalid Protocol" << std::endl;
		return INVALID_SOCKET;
	}

	return s;
}