#include <iostream>
#include <fstream>
#include <string>
#include "networking/networking.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    Networking::initWinSock();
    Networking::ServerSocket serverSocket("8000");

    

    

    

    // No longer need server socket
    

#define DEFAULT_BUFLEN 65536

    char recvbuf[DEFAULT_BUFLEN];
    int iSendResult;
    int recvbuflen = DEFAULT_BUFLEN;


    std::ifstream file("index.html");  // Open the file

    if (!file.is_open()) {
        std::cerr << "Error: Could not open index.html" << std::endl;
        return 1;
    }

    std::string fileContents((std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));
    
    file.close();

    // Receive until the peer shuts down the connection
    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            for (int i = 0; i < iResult; ++i) {
                std::cout << recvbuf[i];
            }
            std::cout << std::endl;
            std::string response;
            response += "HTTP/1.1 200 OK\n";
            response += "\n";
            response += fileContents;
            // Echo the buffer back to the sender
            iSendResult = send(ClientSocket, response.c_str(), response.size(), 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    // shutdown the send half of the connection since no more data will be sent
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    Networking::winSockCleanup();

    return 0;
}