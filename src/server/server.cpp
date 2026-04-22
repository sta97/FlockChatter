#include "server.hpp"
#include <vector>
#include <sodium.h>
#include <stdexcept>
#include <fstream>
#include <iostream>

namespace Server {
void checkServername()
{
	std::ifstream file("servername.txt");
	std::string servername;
	if (file.good())
		file >> servername;
	if (servername.size() == 0)
	{
		std::cout << "Enter server name: ";
		std::cin >> servername;
		std::ofstream file("servername.txt");
		file << servername;
	}
}

    void Client::send(std::string message)
	{
		std::string msg = message.substr(1);
		size_t ciphertextLen = crypto_box_SEALBYTES + msg.size();
		std::vector<char> ciphertext;
		ciphertext.resize(ciphertextLen, 0);
		crypto_box_seal((unsigned char *)ciphertext.data(), (unsigned char *)msg.c_str(), msg.size(), (unsigned char *)clientPublicKey.c_str());
		ciphertext.push_back(0);
		std::string reply = message[0] + std::string(ciphertext.data());
		socket.send(reply);
	}

    std::string Client::recv()
	{
		std::string message = socket.recv();
		if(message.size() == 0)
			return message;
		if(message[0] == Networking::MessageTypes::ExchangePublicKey)
		{
			clientPublicKey = message.substr(1);
			std::string reply = (char)Networking::MessageTypes::ExchangePublicKey + serverPublicKey;
			send(reply);
			return "";
		} else {
			char type = message[0];
			std::string msg = message.substr(1);
			size_t ciphertextLen = msg.size() - crypto_box_SEALBYTES;
			std::vector<char> decrypted;
			decrypted.resize(ciphertextLen);
			if(crypto_box_seal_open((unsigned char *)decrypted.data(), (unsigned char *)msg.c_str(), msg.size(), (unsigned char *)serverPublicKey.c_str(), (unsigned char *)serverPrivateKey.c_str()) != 0)
				throw std::runtime_error("crypto_box_seal_open() failed to decrypt ciphertext");
			decrypted.push_back(0);
			std::string decryptedMessage = type + std::string(decrypted.data());
			return decryptedMessage;
		}
	}
}