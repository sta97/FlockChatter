#include <iostream>
#include <fstream>
#include <string>
#include "networking/networking.hpp"
#include "http/http.hpp"

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

    std::cout << "listening at http://127.0.0.1:8000/" << std::endl;

    std::string index = loadFromFile("index.html");

    std::string icon = loadFromFile("favicon.ico");

    std::string responseIndex;
    responseIndex += "HTTP/1.1 200 OK\n";
    responseIndex += "\n";
    responseIndex += index;

    std::string responseFavicon;
    responseFavicon += "HTTP/1.1 200 OK\n";
    responseFavicon += "\n";
    responseFavicon += icon;

    std::string response404 = "HTTP/1.1 404 Not Found";

    while (true) {
        Networking::ClientSocket socket = serverSocket.accept();
        std::string message = socket.recv();
        std::cout << "received:" << std::endl << message << std::endl << std::endl;
        std::string path = http::getPath(message);
        std::cout << "path: " << path << std::endl << std::endl;
        if (path == "/")
            socket.send(responseIndex);
        else if (path == "/favicon.ico")
            socket.send(responseFavicon);
        else
            socket.send(response404);
    }

    Networking::winSockCleanup();

    return 0;
}