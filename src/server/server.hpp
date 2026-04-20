#include "../networking/networking.hpp"

namespace Server {
void checkServername();
struct Client {
    Networking::ClientSocket socket;
    std::string publicKey;
    int voiceChannel = -1;
    int typingChannel = -1;
    int session = -1;
}
}