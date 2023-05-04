#include "parsing.hpp"

int main(int argc, char **argv) {
	if (argc == 2)
	{
		parsing file((std::string(argv[1])));
		file.readAndParse();
	}
	else 
	{
		std::cout <<"Error : Invalid Arguments" << std::endl;
		exit(1);
	}

}