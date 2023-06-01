#ifndef PARSING_HPP_
#define PARSING_HPP_

#pragma once

#include <sys/_types/_size_t.h>
#include <iostream>
#include <sys/_types/_socklen_t.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
/* hamid's includes */
# include <map>
# include <stack>
# include <fstream>
# include <sstream>

/* forward declaration */
class Client;
class Server;
class Socket;
class User_data;
class request;
class response;
struct data_header;
// #include "user_data.hpp"

# define INSERV 10
# define OUTSERV 5
// macros

#define BACKLOG 10
#define BUFFER_SIZE 2000


struct data_serv {
	std::map<std::string, std::map<std::string, std::string> > location;
	std::string server_name;
	std::string port;
	std::string auto_index;
	std::string root_dir;
	long long	max_body_size;
	std::map<int, std::string> errors;

};

class parsing
{
	private:
		std::string filename;
		std::stack <std::string> brackets;
		std::stack<std::string> end_brackets;
		std::ifstream file;
	public:
		static std::map<std::string, std::string> mime_type;
		std::vector<data_serv> servers_data;
	
		parsing(std::string name);

		int			check_ints(std::string str);
		std::string	trim(std::string line, std::string whitespaces);
		void		save_data(std::vector <std::string> server);
		std::string reduce(std::string buff, std::string whitespaces, std::string to_replace);
		void		readAndParse();
		static void 		mime();

		~parsing();
};



#endif