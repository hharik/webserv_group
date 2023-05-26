

#include "parsing.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include "request.hpp"
int main(int argc, char **argv) {
	if (argc == 2)
	{
		parsing file((std::string(argv[1])));
		file.mime();
		file.readAndParse();
		request req;
		char buffer[5000];
		int socketfd;
		struct sockaddr_in my_add;
		socketfd = socket(PF_INET, SOCK_STREAM, 0);
		my_add.sin_family = AF_INET;
		my_add.sin_port = htons(4040);
		my_add.sin_addr.s_addr = inet_addr("127.0.0.1");
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
		
			struct sockaddr_in host_addr;
			new_socket = accept(socketfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addr);
			std::string hff;
		while (true)
		{
			// fcntl(socketfd, F_SETFL, O_NONBLOCK);
			// char ip_da[25];
			// inet_ntop(AF_INET, &host_addr.sin_addr, ip_da, INET_ADDRSTRLEN);

			int valread = recv(new_socket, buffer, sizeof(buffer), 0);
			if (valread < 0){
				std::cout << "faileeeeed\n";
			}

			// std::cout << "hello  << " << std::endl;
			// std::string f = buffer;
			// std::cout << " here is the client ip "  <<  ip_da << std::endl;
			// std::string te = buffer;
			// req.parse(te);
			// std::cout << valread << "|" << buffer << "|" << std::endl;
			// int j = 0;
			if (valread > 0)
			{
				hff.append(buffer, valread);
				req.parse(hff);
			}
				
				// while (j < 2432)
			// {
			// 	std::cout << buffer[j] << std::endl;
			// 	j++;
			// }
			// sleep(2);
			// memset(buffer, 0, 8000);
			// std::cout << "lll\n";
			// valread = recv(new_socket, buffer, 7999, 0);
			// // buffer[valread] = 0;
			// // std::cout << buffer << std::endl;
			// exit(1);
			// std::string a = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
			// send(new_socket, a.c_str(), a.size(), 0);
			// close(new_socket);
		}
	}
	else
	{
		std::cout <<"Error : Invalid Arguments" << std::endl;
		exit(1);
	}
}