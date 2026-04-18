#include "login.hpp"
#include <filesystem>
#include <iostream>
#include <random>
#include <fstream>
#include <stdexcept>
#include <sodium.h>

namespace login {

    void initSodium() {
        if (sodium_init() < 0)
            throw std::runtime_error("sodium_init() failed");
    }

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
                if (crypto_pwhash_str_verify(u.password.c_str(), password.c_str(), password.size()) == 0)
                    return true;
                else
                    return false;
        return false;
    }

    std::string UserDatabase::findUsername(int id) {
        for (User u : users)
            if (u.id == id)
                return u.username;
        return "";
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
        char hashed_password[crypto_pwhash_STRBYTES];
        if (crypto_pwhash_str(hashed_password, password.c_str(), password.size(), crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0)
            throw std::runtime_error("crypto_pwhash_str() out of memory");
        User u;
        u.username = username;
        u.password = std::string(hashed_password, crypto_pwhash_STRBYTES);
        u.id = id;
        users.push_back(u);
        saveUsers();
        return true;
    }

    void SessionDatabase::loadSessions() {
        sessions.clear();
        if (!std::filesystem::exists("sessions.txt"))
            return;
        std::ifstream file("sessions.txt");
        std::string line;
        while (std::getline(file, line)) {
            int sessionID = std::stoi(line);
            std::getline(file, line);
            int userID = std::stoi(line);
            std::getline(file, line);
            std::time_t sessionStart = std::stoll(line);
            sessions.insert({sessionID,{userID, sessionStart}});
        }
        file.close();
    }

    void SessionDatabase::saveSessions() {
        std::ofstream file("sessions.txt");
        for (const auto& pair : sessions) {
            file << pair.first << "\n";
            file << pair.second.first << "\n";
            file << pair.second.second << "\n";
        }
        file.close();
    }

    SessionDatabase::SessionDatabase() {
        loadSessions();
    }

    SessionDatabase::~SessionDatabase() {
        saveSessions();
    }

    int SessionDatabase::startSession(int userID) {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<int> dist(
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max()
        );

        int sessionID = dist(gen);

        std::time_t sessionStart = std::time(nullptr);

        sessions.insert({ sessionID,{userID, sessionStart} });

        saveSessions();

        return sessionID;
    }

    void SessionDatabase::endSession(int sessionID) {
        sessions.erase(sessionID);
        saveSessions();
    }

    std::time_t SessionDatabase::sessionAge(int sessionID) {
        return sessions[sessionID].second;
    }

    int SessionDatabase::getUserID(int sessionID) {
        return sessions[sessionID].first;
    }
}