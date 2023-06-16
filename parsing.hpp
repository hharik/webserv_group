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
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <dirent.h>
#include <signal.h>
#include <stdlib.h>
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
#define BUFFER_SIZE 5000
#define CMP(path)(strcmp(path, ".") && strcmp(path, ".."))
#define VALID_STATUS(x) x >= 100 && x < 600 ? true : false
#define FAILED(x) x >= 400 ? true : false
#define SUCCESS(x) x < 400 ? true : false

struct data_serv {
	std::map<std::string, std::map<std::string, std::string> > location;
	std::string server_name;
	std::string port;
	std::string index;
	std::string root_dir;
	std::string cgi_mode;
	long long	max_body_size;
	std::map<int, std::string> errors;
	data_serv() : max_body_size(-1) { }
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
		static std::map<std::string, std::string> mime_types_ay;

		std::vector<data_serv> servers_data;
	
		parsing(std::string name);

		int			check_ints(std::string str);
		std::string	trim(std::string line, std::string whitespaces);
		int		save_data(std::vector <std::string> server);
		std::string reduce(std::string buff, std::string whitespaces, std::string to_replace);
		void		readAndParse();
		static void 		mime();
	
		~parsing();
};



#endif