#include <iostream>
#include <fstream>
#include <string>
#include "networking/networking.hpp"
#include "login/login.hpp"
#include <windows.h>
#include <sodium.h>

std::string decrypt(std::string message, std::string clientPublicKey, std::string clientPrivateKey)
{
	char type = message[0];
	std::string msg = message.substr(1);
	size_t ciphertextLen = msg.size() - crypto_box_SEALBYTES;
	if (msg.size() < crypto_box_SEALBYTES)
	{
		std::cout << "message of size " << msg.size() << " shorter than crypto_box_SEALBYTES";
		return "";
	}
	std::vector<char> decrypted;
	decrypted.resize(ciphertextLen);
	if (crypto_box_seal_open((unsigned char*)decrypted.data(), (unsigned char*)msg.c_str(), msg.size(), (unsigned char*)clientPublicKey.c_str(), (unsigned char*)clientPrivateKey.c_str()) != 0)
	{
		std::cout << "crypto_box_seal_open() failed to decrypt ciphertext" << std::endl;
		return "";
	}
	decrypted.push_back(0);
	std::string decryptedMessage = type + std::string(decrypted.data());
	return decryptedMessage;
}

std::string encrypt(std::string message, std::string serverPublicKey)
{
	std::string msg = message.substr(1);
	size_t ciphertextLen = crypto_box_SEALBYTES + msg.size();
	std::vector<char> ciphertext;
	ciphertext.resize(ciphertextLen, 0);
	crypto_box_seal((unsigned char*)ciphertext.data(), (unsigned char*)msg.c_str(), msg.size(), (unsigned char*)serverPublicKey.c_str());
	ciphertext.push_back(0);
	std::string reply = message[0] + std::string(ciphertext.data());
	return reply;
}

int main() {
	login::initSodium();

	Networking::initWinSock();
	

	bool connected = false;
	bool loggedIn = false;

	std::string clientPrivateKey, clientPublicKey, serverPublicKey, serverAddress, username, password;
	clientPublicKey.resize(crypto_box_PUBLICKEYBYTES, 0);
	clientPrivateKey.resize(crypto_box_SECRETKEYBYTES, 0);
	crypto_box_keypair((unsigned char*)clientPublicKey.c_str(), (unsigned char*)clientPrivateKey.c_str());

	std::cout << "server address: ";
	std::cin >> serverAddress;

	//std::cout << "username: ";
	//std::cin >> username;

	//std::cout << "password: ";
	//std::cin >> password;

	std::cout << "connecting to server..." << std::endl;

	Networking::ClientSocket socket(serverAddress, "8000");
	//Sleep(1000);

	std::cout << "Exchanging public keys..." << std::endl;

	socket.send((char)Networking::MessageTypes::ExchangePublicKey + clientPublicKey);

	std::string message = socket.recv();

	while (message.size() == 0)
	{
		//std::cout << "waiting..." << std::endl;
		message = socket.recv();
		//Sleep(1000);
	}

	std::cout << message << std::endl;

	message = decrypt(message, clientPublicKey, clientPrivateKey);

	if(message.size() > 2)
		serverPublicKey = message.substr(1);

	std::cout << "serverPublicKey.size(): " << serverPublicKey.size() << std::endl;

	std::cout << "Getting server name..." << std::endl;

	socket.send(std::string(1, Networking::MessageTypes::GetServerName));

	message = "";
	while (message.size() == 0) {
		//std::cout << "waiting..." << std::endl;
		message = socket.recv();
		//Sleep(1000);
	}

	message = decrypt(message, clientPublicKey, clientPrivateKey);

	if(message.size() > 2)
		std::cout << "server name: " << message.substr(1) << std::endl;

	Networking::winSockCleanup();

	Sleep(10000);

	return 0;
}
