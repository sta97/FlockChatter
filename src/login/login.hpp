#include <string>
#include <vector>

namespace login {
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
        std::vector<User> users;
        void loadUsers();
        void saveUsers();
        public:
        UserDatabase();
        ~UserDatabase();
        std::string findUsername(unsigned int id);
        std::string findID(std::string username);
        bool login(std::string username, std::string password);
        bool addUser(std::string username, std::string password);
    };
}