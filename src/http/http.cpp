#include "http.hpp"
#include <iostream>

namespace http
{
    std::string getPath(std::string message)
    {
        size_t start = message.find(' ');
        size_t end = message.find(' ', start+1);
        if (start == std::string::npos || end == std::string::npos)
            return "";
        std::string path;
        for(size_t i = start+1; i < end; ++i)
            path += message[i];
        return path;
    }

    std::string createResponse(std::string body, std::string contentType, std::vector<std::pair<std::string, std::string>> setCookies)
    {
        std::string response;
        response += "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: " + contentType + "\r\n";
        response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
        for (auto cookie : setCookies)
            response += "Set-Cookie: " + cookie.first + "=" + cookie.second + "\r\n";
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

    std::vector<std::pair<std::string, std::string>> parseCookies(std::string message)
    {
        std::vector<std::pair<std::string, std::string>> parsed;
        size_t next = 0;
        while (message.find("Cookie:", next) != std::string::npos) {
            size_t cookieStart = message.find("Cookie:", next);
            cookieStart = message.find(" ", cookieStart) + 1;
            std::string cookieName = "";
            for (size_t i = cookieStart; message[i] != '='; ++i)
                cookieName += message[i];
            cookieStart = message.find("=", cookieStart) + 1;
            std::string cookieData = "";
            for (size_t i = cookieStart; message[i] != '\r'; ++i)
                cookieData += message[i];
            parsed.push_back(std::make_pair(cookieName, cookieData));
            next = message.find("\n", cookieStart);
        }
        return parsed;
    }
}