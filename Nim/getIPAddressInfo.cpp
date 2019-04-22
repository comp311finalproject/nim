/*------------------------------------------------------------------------------
 * getIPAddressInfo -	Accesses the network settings for the first network
						adapter that is active and
						retrieves the associated IP and broadcast addressess
			 (Function returns a status value: 0 == success -1 == failure).
 *------------------------------------------------------------------------------*/
#include <iostream>
#include <WS2tcpip.h>
#include <iphlpapi.h>

#pragma comment (lib, "iphlpapi.lib")

int IPStr2Int(const char*, unsigned long&);
int IPInt2Str(unsigned long, char*);

int getIPAddressInfo(char *myIPAddress, char *myBroadcastAddress)
{
	// [OUT] myIpAddress		: null-terminated string of dotted-decimal IP Address
	// [OUT] myBroadcastAddress : null-terminated string of broadcast address
	//  The receiving char arrays need to be large enough to hold at least 16 characters

	// Return value:  This function returns 0 if successful; otherwise returns -1

	// NOTE: This code only returns the information for the first adapter
	//		found that has a non-zero IP address associated.

#define MAXADDRSIZE 16
	int status = -1;

	// Make an initial call to GetAdaptersInfo to get
	// the necessary size needed in the ulOutBufLen variable
	PIP_ADAPTER_INFO pAdapterInfo = (IP_ADAPTER_INFO *)HeapAlloc(GetProcessHeap(), 0, sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL) {
		std::cout << "Error allocating memory needed by getIPAddressInfo()" << std::endl;
		return status;
	}
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
		// Release initial allocation of heap memory and request more appropriately sized memory
		HeapFree(GetProcessHeap(), 0, pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)HeapAlloc(GetProcessHeap(), 0, ulOutBufLen);
		if (pAdapterInfo == NULL) {
			std::cout << "Error allocating memory needed by getIPAddressInfo()" << std::endl;
			return status;
		}
	}

	DWORD dwRetVal = 0;
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
		PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
		// Loop through all of the adapters looking for the 1st one with non-zero IPAddress
		bool found = false;
		while (pAdapter && !found) {
			unsigned long IPAddress = 0;
			if (IPStr2Int(pAdapter->IpAddressList.IpAddress.String, IPAddress) == 0) {
				if (IPAddress > 0) {
					found = true;
					char Address[MAXADDRSIZE] = "";
					if (IPInt2Str(IPAddress, Address) == 0) {
						strcpy_s(myIPAddress, MAXADDRSIZE, Address);
						unsigned long subnetMask = 0;
						if (IPStr2Int(pAdapter->IpAddressList.IpMask.String, subnetMask) == 0) {
							unsigned long BcastMask = subnetMask ^ 0xffffffff;
							unsigned long broadcastAddress = IPAddress | BcastMask;
							if (IPInt2Str(broadcastAddress, Address) == 0) {
								strcpy_s(myBroadcastAddress, MAXADDRSIZE, Address);
								status = 0;
							}
						}
					}
				}
			}
			pAdapter = pAdapter->Next;
		}
		if (!found) {
			std::cout << "Error in getIPAddressInfo(): No active network adapter found" << std::endl;
		}
	}
	else {
		std::cout << "Error in getIPAddressInfo(): GetAdaptersInfo failed with error: " << dwRetVal << std::endl;
	}
	if (pAdapterInfo)
		HeapFree(GetProcessHeap(), 0, pAdapterInfo);

	return status;
}

int IPStr2Int(const char* IPStr, unsigned long &IPInt) {
	int status = -1;
	struct sockaddr_in socketInfo;
	int result = inet_pton(AF_INET, IPStr, &(socketInfo.sin_addr));
	if (result > 0) {
		IPInt = socketInfo.sin_addr.S_un.S_addr;
		status = 0;
	}
	return status;
}

int IPInt2Str(unsigned long IPInt, char *IPStr) {
	int status = -1;
	in_addr ip_addr;
	ip_addr.S_un.S_addr = IPInt;
	if (inet_ntop(AF_INET, &ip_addr.S_un.S_addr, IPStr, 16) != NULL) {
		status = 0;
	}
	else {
		std::cout << "Problem with inet_ntop() function in getIPAddressInfo()" << std::endl;
	}
	return status;
}