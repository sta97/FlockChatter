

#ifdef _WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "networking.hpp"
#include <stdexcept>
#include <vector>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

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

	ClientSocket::ClientSocket() {
		socket = INVALID_SOCKET;
	}

	ClientSocket::ClientSocket(SOCKET socket) {
		u_long mode = 1;
		ioctlsocket(socket, FIONBIO, &mode);
		this->socket = socket;
	}

	ClientSocket::ClientSocket(ClientSocket&& other) noexcept
		: socket(other.socket)
	{
		other.socket = INVALID_SOCKET;
	}

	ClientSocket& ClientSocket::operator=(ClientSocket&& other) noexcept
	{
		if (this != &other)
		{
			if (socket != INVALID_SOCKET)
			{
				closesocket(socket);
			}

			socket = other.socket;
			other.socket = INVALID_SOCKET;
		}
		return *this;
	}

	ClientSocket::ClientSocket(std::string address, std::string port)
	{
		struct addrinfo* result = NULL,
			* ptr = NULL,
			hints;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		// Resolve the server address and port
		int iResult = getaddrinfo(address.c_str(), port.c_str(), &hints, &result);
		if (iResult != 0)
			throw std::runtime_error("getaddrinfo failed: " + std::to_string(iResult));
		
		ptr = result;

		this->socket = ::socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (this->socket == INVALID_SOCKET) {
			freeaddrinfo(result);
			throw std::runtime_error("Error at socket(): " + std::to_string(WSAGetLastError()));
		}

		u_long mode = 1;
		ioctlsocket(this->socket, FIONBIO, &mode);

		iResult = connect(this->socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				std::cout << "connect() failed" << std::endl;
				closesocket(this->socket);
				this->socket = INVALID_SOCKET;
			}
		}

		freeaddrinfo(result);

		if (socket == INVALID_SOCKET)
			throw std::runtime_error("Unable to connect to server!");
	}

	bool ClientSocket::isValid() {
		if (socket == INVALID_SOCKET)
			return false;
		else
			return true;
	}

	void ClientSocket::send(std::string data) {
		std::cout << "sending data of size: " << data.size() << std::endl;
		int iSendResult = ::send(socket, data.c_str(), (int) data.size(), 0);
		std::cout << "sent data of size: " << iSendResult << std::endl;
		if (iSendResult == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				return;
			std::cout << "send failed: " << WSAGetLastError() << std::endl;
			closesocket(socket);
			socket = INVALID_SOCKET;
		}
	}

	std::string ClientSocket::recv() {
		std::vector<char> buffer;
		buffer.resize(1048576);

		int result = ::recv(socket, buffer.data(), ((int)buffer.size()) - 1, 0);
		std::cout << "recv data of size " << result << std::endl;
		if (result > 0) {
			return std::string(buffer.data(), result);
		}
		else if (result == 0) {
			return std::string();
		}
		else {
			if (WSAGetLastError() == WSAEWOULDBLOCK)
				return std::string();
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
			closesocket(socket);
			this->socket = INVALID_SOCKET;
			return std::string();
		}
	}

	ClientSocket::~ClientSocket() {
		std::cout << "client socket closed due to RAII " << socket << std::endl;
		shutdown(socket, SD_SEND);
		closesocket(socket);
		socket = INVALID_SOCKET;
	}

	ServerSocket::ServerSocket(PCSTR port)
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

		socket = INVALID_SOCKET;

		// Create a SOCKET for the server to listen for client connections

		socket = ::socket(result->ai_family, result->ai_socktype, result->ai_protocol);

		if (socket == INVALID_SOCKET) {
			freeaddrinfo(result);
			throw std::runtime_error("Error at socket(): " + std::to_string(WSAGetLastError()));
		}

		u_long mode = 1;
		ioctlsocket(socket, FIONBIO, &mode);

		// Setup the TCP listening socket
		iResult = bind(socket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			freeaddrinfo(result);
			closesocket(socket);
			throw std::runtime_error("bind failed with error: " + std::to_string(WSAGetLastError()));
		}

		freeaddrinfo(result);

		if (listen(socket, SOMAXCONN) == SOCKET_ERROR) {
			closesocket(socket);
			throw std::runtime_error("Listen failed with error: " + std::to_string(WSAGetLastError()));
		}
	}

	ClientSocket ServerSocket::accept()
	{
		SOCKET clientSocket;

		clientSocket = INVALID_SOCKET;

		// Accept a client socket
		clientSocket = ::accept(socket, NULL, NULL);
		//if (clientSocket == INVALID_SOCKET) {
		//	throw std::runtime_error("accept failed: " + std::to_string(WSAGetLastError()));
		//}

		return ClientSocket(clientSocket);
	}

	ServerSocket::~ServerSocket()
	{
		closesocket(socket);
	}
}

#endif



#ifdef __unix__

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <cstdio>

#include "networking.hpp"
#include <stdexcept>
#include <vector>
#include <string>

namespace Networking
{
	int initWinSock()
	{
		// No initialization needed on Linux.
		return 0;
	}

	void winSockCleanup()
	{
		// No cleanup needed on Linux.
	}

	ClientSocket::ClientSocket()
	{
		socket = -1;
	}

	ClientSocket::ClientSocket(int socket)
	{
		fcntl(socket, F_SETFL, O_NONBLOCK);
		this->socket = socket;
	}

	void ClientSocket::send(std::string data)
	{
		ssize_t result = ::send(socket, data.c_str(), data.size(), 0);
		if (result == -1)
		{
			throw std::runtime_error("send failed: " + std::string(std::strerror(errno)));
		}
	}

	std::string ClientSocket::recv()
	{
		std::vector<char> buffer(1048576);

		ssize_t result = ::recv(socket, buffer.data(), buffer.size() - 1, 0);
		if (result > 0)
		{
			return std::string(buffer.data(), static_cast<size_t>(result));
		}
		else if (result == 0)
		{
			return std::string();
		}
		else
		{
			throw std::runtime_error("recv failed: " + std::string(std::strerror(errno)));
		}
	}

	ClientSocket::~ClientSocket()
	{
		if (socket != -1)
		{
			::shutdown(socket, SHUT_WR);
			::close(socket);
		}
	}

	ServerSocket::ServerSocket(const char* port)
	{
		struct addrinfo hints;
		struct addrinfo* result = nullptr;

		std::memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		int iResult = ::getaddrinfo(nullptr, port, &hints, &result);
		if (iResult != 0)
		{
			throw std::runtime_error("getaddrinfo failed: " + std::string(gai_strerror(iResult)));
		}

		socket = -1;

		socket = ::socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (socket == -1)
		{
			::freeaddrinfo(result);
			throw std::runtime_error("socket failed: " + std::string(std::strerror(errno)));
		}

		fcntl(socket, F_SETFL, O_NONBLOCK);

		int opt = 1;
		if (::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		{
			::freeaddrinfo(result);
			::close(socket);
			throw std::runtime_error("setsockopt failed: " + std::string(std::strerror(errno)));
		}

		iResult = ::bind(socket, result->ai_addr, result->ai_addrlen);
		if (iResult == -1)
		{
			::freeaddrinfo(result);
			::close(socket);
			throw std::runtime_error("bind failed: " + std::string(std::strerror(errno)));
		}

		::freeaddrinfo(result);

		if (::listen(socket, SOMAXCONN) == -1)
		{
			::close(socket);
			throw std::runtime_error("listen failed: " + std::string(std::strerror(errno)));
		}
	}

	ClientSocket ServerSocket::accept()
	{
		int clientSocket = ::accept(socket, nullptr, nullptr);
		if (clientSocket == -1)
		{
			throw std::runtime_error("accept failed: " + std::string(std::strerror(errno)));
		}

		return ClientSocket(clientSocket);
	}

	ServerSocket::~ServerSocket()
	{
		if (socket != -1)
		{
			::close(socket);
		}
	}
}

#endif