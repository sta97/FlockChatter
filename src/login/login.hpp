#include <string>
#include <vector>
#include <unordered_map>
#include <ctime>

namespace login {
    void initSodium();

    enum PermissionLevel {
        OWNER = 0,
        ADMIN = 1,
        USER = 2
    };

    class User {
        public:
        int id = -1;
        std::string username;
        std::string password;
        PermissionLevel permissions = PermissionLevel::USER;
    };

    class UserDatabase {
        void loadUsers();
        void saveUsers();
        std::vector<User> users;
        public:
        UserDatabase();
        ~UserDatabase();
        std::string findUsername(int id);
        int findID(std::string username);
        bool login(std::string username, std::string password);
        bool addUser(std::string username, std::string password);
    };

    class SessionDatabase {
        void loadSessions();
        void saveSessions();
        std::unordered_map<int, std::pair<int, std::time_t>> sessions; // <SessionID, <UserID, session start time>>
    public:
        SessionDatabase();
        ~SessionDatabase();
        int startSession(int userID);
        void endSession(int sessionID);
        std::time_t sessionAge(int sessionID);
        int getUserID(int sessionID);
    };
}