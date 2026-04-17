#include <string>

#ifdef _WIN32

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

#endif



#ifdef __unix__

class ClientSocket {
public:
    ClientSocket();
    ClientSocket(int socket);
    void send(std::string data);
    std::string recv();
    ~ClientSocket();
private:
    int socket;
};

class ServerSocket {
public:
    ServerSocket(const char* port);
    ClientSocket accept();
    ~ServerSocket();
private:
    int socket;
};

#endif