
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
}