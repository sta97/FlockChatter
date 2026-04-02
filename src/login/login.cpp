#include "login.hpp"
#include <filesystem>
#include <iostream>

namespace login {
    UserDatabase::UserDatabase()  {
        loadUsers();
        if(users.size() == 0) {
            std::cout << "No users currently exist. Please create inital user as owner." << std::endl;
            std::cout << "Username: ";
            std::string username, password;
            std::cin >> username;
            std::cout << "Password: ";
            std::cin >> password;
            addUser(username, password);
        }
    }

    UserDatabase::~UserDatabase() {
        saveUsers();
    }

    void UserDatabase::loadUsers() {
        users.clear();
        if (!std::filesystem::exists("users.txt"))
            return;
    }

    void UserDatabase::saveUsers() {
        
    }

    bool UserDatabase::addUser(std::string username, std::string password) {
        return false;
    }
}