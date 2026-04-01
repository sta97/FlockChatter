#include "http.hpp"

namespace http
{
    std::string getPath(std::string message)
    {
        int start = message.find(' ');
        int end = message.find(' ', start+1);
        std::string path;
        for(int i = start+1; i < end; ++i)
            path += message[i];
        return path;
    }
}