#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "networking.hpp"

namespace Networking
{
	int initWinSock()
	{
		WSADATA wsaData;

		int iResult;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			printf("WSAStartup failed: %d\n", iResult);
			return 1;
		}
		return 0;
	}

	void winSockCleanup() {
		WSACleanup();
	}
}