#include "../networking/networking.hpp"

namespace Server {
void checkServername();
struct Client {
    Networking::ClientSocket socket;
    std::string clientPublicKey;
    std::string serverPublicKey;
    std::string serverPrivateKey;
    int voiceChannel = -1;
    int typingChannel = -1;
    int session = -1;
    void send(std::string message);
    std::string recv();
}
}