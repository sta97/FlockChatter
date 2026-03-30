#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "networking.hpp"
#include <stdexcept>

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

	class ServerSocket
	{
		SOCKET ListenSocket;
	public:
		ServerSocket(PCSTR port)
		{
			struct addrinfo* result = NULL, * ptr = NULL, hints;

			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;

			// Resolve the local address and port to be used by the server
			int iResult = getaddrinfo(NULL, port, &hints, &result);
			if (iResult != 0) {
				throw std::runtime_error("getaddrinfo failed: " + std::to_string(iResult));
			}

			ListenSocket = INVALID_SOCKET;

			// Create a SOCKET for the server to listen for client connections

			ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

			if (ListenSocket == INVALID_SOCKET) {
				freeaddrinfo(result);
				throw std::runtime_error("Error at socket(): " + std::to_string(WSAGetLastError()));
			}

			// Setup the TCP listening socket
			iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				freeaddrinfo(result);
				closesocket(ListenSocket);
				throw std::runtime_error("bind failed with error: " + std::to_string(WSAGetLastError()));
			}

			freeaddrinfo(result);

			if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
				closesocket(ListenSocket);
				throw std::runtime_error("Listen failed with error: " + std::to_string(WSAGetLastError()));
			}
		}
		ClientSocket acceptClient()
		{
			SOCKET clientSocket;

			clientSocket = INVALID_SOCKET;

			// Accept a client socket
			clientSocket = accept(ListenSocket, NULL, NULL);
			if (clientSocket == INVALID_SOCKET) {
				throw std::runtime_error("accept failed: " + std::to_string(WSAGetLastError()));
			}

			return ClientSocket(clientSocket);
		}
		~ServerSocket()
		{
			closesocket(ListenSocket);
		}
	};
}