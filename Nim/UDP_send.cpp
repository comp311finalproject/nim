/*------------------------------------------------------------
 * UDP_send - Send a string to a host machine using UDP
 *------------------------------------------------------------*/
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "nim.h"

int UDP_send(SOCKET s, const char *buf, int numBytesToSend, const char *host, const char *service)
{	// Parameter List:
	// [IN] s      : SOCKET handle
	// [IN] buf    : Address of NULL-terminated char array containing bytes to be sent to host
	// [IN] numBytesToSend: Integer containing the number of bytes in buf that should be sent
	//                       Caution: This integer should be <= the size of the buf char array!
	// [IN] host   : Address of NULL-terminated char array containing domain name of remote host or IP Address
	//               Broadcast-type IP Address (such as 10.1.255.255) is supported.
	// [IN] service: Address of NULL-terminated char array containing service name or port number to use for transmission

	// Return Value: Number of bytes actually sent.

	//	NOTE: This function requires the <iostream> and WinSock2.h header files
	//        as well as the ws2_32.lib library file.

	struct servent	*pse;		/* pointer to service information entry  */
	sockaddr_in		sin;		/* an Intenet endpoint address           */
	int				numBytes;	/* Number of bytes actually sent         */

	if (s < 0 || s == INVALID_SOCKET) {
		std::cout << "Problem with UDP_send().  Invalid socket handle." << std::endl;
		numBytes = -1;
	}
	else if (numBytesToSend <= 0) {
		std::cout << "Error: Attempting to send an invalid number of bytes to " << host << ":" << service << std::endl;
		numBytes = -1;

	}
	else {
		memset(&sin, 0, sizeof(sin));  /* zero the sockaddr_in struct */

		/* Set the address family to AF_INET (= Internet) */
		sin.sin_family = AF_INET;

		/* Map service name (or number) to port number */
		if (pse = getservbyname(service, "UDP"))
			sin.sin_port = pse->s_port;
		else if ((sin.sin_port = htons((u_short)atoi(service))) == 0) {
			std::cout << "Invalid service request." << std::endl;
			return INVALID_SOCKET;
		}

		/* Map host name to IP address, allowing for dotted quad */
		int result = inet_pton(AF_INET, host, &sin.sin_addr);
		if (result <= 0) {
			// If that doesn't work, try sending a request to DNS using the name of the remote host
			struct addrinfo *result = NULL;
			DWORD dwRetval = getaddrinfo(host, service, 0, &result);
			if (dwRetval == 0) {	// If DNS responded with valid response (ie. dwRetval is 0),  
									//    Copy the returned 32-bit address to clientInfo
				struct sockaddr_in  *sockaddr_ipv4;
				sockaddr_ipv4 = (struct sockaddr_in *) result->ai_addr;
				sin.sin_addr.s_addr = sockaddr_ipv4->sin_addr.S_un.S_addr;
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

		numBytes = sendto(s, buf, numBytesToSend, 0, (LPSOCKADDR)&sin, sizeof(sin));
		if (numBytes <= 0) {
			std::cout << std::endl << "Problem with sendto() function call within UDP_send().  Error Code = " << WSAGetLastError() << std::endl;
		}
	}

	return numBytes;
}