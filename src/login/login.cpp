#include "login.hpp"
#include <filesystem>
#include <iostream>
#include <random>
#include <fstream>

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
            users[0].permissions = PermissionLevel::OWNER;
            saveUsers();
        }
    }

    UserDatabase::~UserDatabase() {
        saveUsers();
    }

    void UserDatabase::loadUsers() {
        users.clear();
        if (!std::filesystem::exists("users.txt"))
            return;
        std::ifstream file("users.txt");
        std::string line;
        while (std::getline(file, line)) {
            User u;
            u.id = std::stoi(line);
            std::getline(file, line);
            u.username = line;
            std::getline(file, line);
            u.password = line;
            std::getline(file, line);
            u.permissions = (PermissionLevel) std::stoi(line);
            users.push_back(u);
        }
        file.close();
    }

    void UserDatabase::saveUsers() {
        std::ofstream file("users.txt");
        for (User u : users) {
            file << u.id << "\n";
            file << u.username << "\n";
            file << u.password << "\n";
            file << u.permissions << "\n";
        }
        file.close();
    }

    bool UserDatabase::login(std::string username, std::string password) {
        for (User u : users)
            if (u.username == username)
                if (u.password == password)
                    return true;
                else
                    return false;
        return false;
    }

    int UserDatabase::findID(std::string username) {
        for (User u : users)
            if (u.username == username)
                return u.id;
        return -1;
    }

    bool UserDatabase::addUser(std::string username, std::string password) {
        int id = 0;
        for (User u : users)
        {
            if (id <= u.id)
                id = u.id + 1;
            if (u.username == username)
                return false;
        }

        User u;
        u.username = username;
        u.password = password;
        u.id = id;
        users.push_back(u);
        saveUsers();
        return true;
    }
}