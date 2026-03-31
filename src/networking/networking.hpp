#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

namespace Networking
{
    int initWinSock();
    void winSockCleanup();
    
    class ClientSocket
    {
        SOCKET socket;
    public:
        ClientSocket();
        ClientSocket(SOCKET socket);
        void send(std::string data);
        std::string recv();
        ~ClientSocket();
    };

    class ServerSocket
    {
        SOCKET socket;
    public:
        ServerSocket(PCSTR port);
        ClientSocket accept();
        ~ServerSocket();
    };
}