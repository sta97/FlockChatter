#include "http.hpp"
#include <iostream>

namespace http
{
    std::string getPath(std::string message)
    {
        size_t start = message.find(' ');
        size_t end = message.find(' ', start+1);
        std::string path;
        for(size_t i = start+1; i < end; ++i)
            path += message[i];
        return path;
    }

    std::string createResponse(std::string body, std::string contentType)
    {
        std::string response;
        response += "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + contentType + "\r\n";
        response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
        response += "\r\n";
        response += body;
        return response;
    }

    std::vector<std::pair<std::string, std::string>> parsePostBody(std::string message)
    {
        std::vector<std::pair<std::string, std::string>> parsed;
        size_t bodyStart = message.find_last_of('\n');
        if (bodyStart == std::string::npos)
            return parsed;
        std::string name, content;
        bool nameSet = true;
        for (size_t i = bodyStart + 1; i < message.size(); ++i)
        {
            if (message[i] == '=')
                nameSet = false;
            else if (message[i] == '&') {
                nameSet = true;
                parsed.push_back(std::make_pair(name, content));
                name = std::string();
                content = std::string();
            }
            else if (nameSet)
                name += message[i];
            else
                content += message[i];
        }
        parsed.push_back(std::make_pair(name, content));
        name = std::string();
        content = std::string();
        return parsed;
    }
}