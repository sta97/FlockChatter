#include <string>
#include <vector>

namespace login {
    class User {
        public:
        std::string id;
        std::string username;
        std::string password;
        bool owner;
        bool admin;
    };

    class UserDatabase {
        std::vector<User> users;
        void loadUsers();
        void saveUsers();
        public:
        UserDatabase();
        ~UserDatabase();
        std::string findUsername(std::string id);
        std::string findID(std::string username);
        bool login(std::string username, std::string password);
        bool addUser(std::string username, std::string password);
    };
}