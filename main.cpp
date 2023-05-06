#include "parsing.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <unistd.h>
#include "request.hpp"
int main(int argc, char **argv) {
	if (argc == 2)
	{
		parsing file((std::string(argv[1])));
		file.readAndParse();
		request req;
		char buffer[8000];
		int socketfd;
		struct sockaddr_in my_add;
		socketfd = socket(PF_INET, SOCK_STREAM, 0);
		my_add.sin_family = AF_INET;
		my_add.sin_port = htons(4040);
		my_add.sin_addr.s_addr = INADDR_ANY;
		int enable = 1;
		int new_socket;
		if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}
		if (bind(socketfd, (struct sockaddr *) &my_add, sizeof(my_add)) == -1)
		{
			perror("setsockopt");
			exit(1);
		}
		std::cout << " Listening on 4040" << std::endl;
		std::vector<data_serv> data = file.get_server_data();
		if (listen(socketfd, 10) != 0)
			std::cout << "Listen is failed " << std::endl;
		
		while (true)
		{
			struct sockaddr_in host_addr;
			char ip_da[25];
			new_socket = accept(socketfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addr);
			inet_ntop(AF_INET, &host_addr.sin_addr, ip_da, INET_ADDRSTRLEN);

			int valread = recv(new_socket, buffer, sizeof(buffer), 0);
			std::cout << " here is the client ip "  <<  ip_da << std::endl;
			// std::cout << buffer << std::endl;
			std::string te = buffer;
			req.parse(te);

			close(new_socket);
		}
	}
	else
	{
		std::cout <<"Error : Invalid Arguments" << std::endl;
		exit(1);
	}
}