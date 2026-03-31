#include <iostream>
#include <fstream>
#include <string>
#include "networking/networking.hpp"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

std::string loadFromFile(std::string filename) {
    std::ifstream file(filename);  // Open the file

    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open " + filename);
    }

    std::string fileContents((std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>()));

    file.close();

    return fileContents;
}

int main() {
    Networking::initWinSock();
    Networking::ServerSocket serverSocket("8000");

    std::string index = loadFromFile("index.html");

    std::string response;
    response += "HTTP/1.1 200 OK\n";
    response += "\n";
    response += index;

    while (true) {
        Networking::ClientSocket socket = serverSocket.accept();
        std::string recv = socket.recv();
        std::cout << "received:" << std::endl << recv << std::endl << std::endl;
        socket.send(response);
    }

    Networking::winSockCleanup();

    return 0;
}