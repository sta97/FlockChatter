#include <string>
#include <vector>

namespace http
{
    std::string getPath(std::string message);
    std::string createResponse(std::string body, std::string contentType);
    std::vector<std::pair<std::string, std::string>> parsePostBody(std::string message);
}