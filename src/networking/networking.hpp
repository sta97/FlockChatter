#include <string>

namespace Networking
{
    int initWinSock();
    void winSockCleanup();
    
    class ClientSocket
    {
        ClientSocket(SOCKET socket);
        void send(std::string data);
        std::string recv();
        ~ClientSocket();
    };

    class ServerSocket
    {
        ServerSocket(int port);
        ClientSocket accept();
        ~ServerSocket();
    };
}