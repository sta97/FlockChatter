#include "helpers.hpp"
#include <fstream>

namespace Helpers {
std::string loadFromFile(std::string filename) {
	std::ifstream file(filename, std::ios::binary);  // Open the file

	if (!file.is_open()) {
		throw std::runtime_error("Error: Could not open " + filename);
	}

	std::string fileContents((std::istreambuf_iterator<char>(file)),
		(std::istreambuf_iterator<char>()));

	file.close();

	return fileContents;
}
}