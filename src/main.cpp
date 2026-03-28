#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream file("index.html");  // Open the file

    if (!file.is_open()) {
        std::cerr << "Error: Could not open index.html" << std::endl;
        return 1;
    }

    std::string line;

    // Read and print file line by line
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }

    file.close();
    return 0;
}