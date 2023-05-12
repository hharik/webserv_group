#ifndef PARSING_HPP_
#define PARSING_HPP_
# include <iostream>
# include <fstream>
# include <ostream>
# include <vector>
# include <stack>
# include <map>
# include <sstream>

std::map<std::string, std::string> mime_type;
struct data_serv {
	std::map<std::string, std::map<std::string, std::string> > location;
	std::map <std::string, std::string> default_data;
};

class parsing
{
	private:
		std::string filename;
		std::stack <std::string> brackets;
		std::ifstream file;
		std::vector<data_serv> servers_data;
	public:
		int check_ints(std::string str) {
			for (std::string::iterator it = str.begin(); it != str.end() ; it++)
			{
				if (!isdigit(*it))
					return -1;
			}
			return 0;
		}
		parsing(std::string name) : filename(name), file(), servers_data() {
		}
		std::string trim(std::string line, std::string whitespaces)
		{
			std::size_t strbegin = line.find_first_not_of(whitespaces);
			if (strbegin == std::string::npos)
				return " ";
			std::size_t strend =  line.find_last_not_of(whitespaces);
			std::size_t strRange = strend - strbegin + 1;
			return line.substr(strbegin, strRange);
		}
		std::string reduce(std::string buff, std::string whitespaces, std::string to_replace = " ") {
			buff = trim(buff, " \t\r\v\f\n");
			std::size_t beginSpace = buff.find_first_of(whitespaces);
			std::size_t endSpace;
			while (beginSpace != std::string::npos)
			{
				endSpace = buff.find_first_not_of(whitespaces, beginSpace);
				std::size_t range = endSpace - beginSpace;
				buff.replace(beginSpace, range, to_replace);
				beginSpace = buff.find_first_of(whitespaces, beginSpace + 1);
			}
			size_t che;
			if ((che = buff.find_first_of("{")) != std::string::npos)
			{
				if (buff.find("{", che + 1) != std::string::npos || buff.find("}", che + 1) != std::string::npos)
				{
					std::cout << "Error Brackets " << std::endl;
					exit(1);
				}
				brackets.push("{");
			}
			if ((che = buff.find("}")) != std::string::npos)
			{
				if (buff.find("}", che  + 1) != std::string::npos || buff.find("{", che + 1)  != std::string::npos)
				{
					std::cout << "Error Brackets " << std::endl;
					exit(1);
				}
				brackets.pop();
			}
			return buff;
		}
		void save_data(std::vector <std::string> server)
		{
			data_serv temp;
			std::map<std::string, std::map<std::string, std::string> > locations;
			size_t pos;

			std::vector <std::string>::iterator it = server.begin();
			// std::cout << " * ** * *" <<  *(it + 1) << std::endl;

			while (it != server.end())
			{
				// save default data
				std::string _value;
				pos = it->find(" ");
				// std::cout << "*" << *it	<< "*num " << std::endl;
				if ((it->find("server_name ")) != std::string::npos)
				{
					_value = it->substr(pos + 1);
					// std::cout << _value <<  " here is the server_name " << std::endl;
					if (_value.find(":") == std::string::npos && _value.find_last_of(".") == std::string::npos)
					{
						std::cout << "Server_name not provided and doesnt have a specific port" << std::endl;
						exit(1);
					}
					temp.default_data.insert(std::make_pair("server_name", _value));
				}
				else if ((it->find("max_body_size ")) != std::string::npos)
				{
					_value = it->substr(pos + 1);
					if (check_ints(_value) == -1)
					{
						std::cout << "Error : Max body size isnt int" << std::endl;
						exit(1);
					}
					temp.default_data.insert(std::make_pair("max_body_size", _value));
				}
				else if ((it->find("listen ")) != std::string::npos)
				{
					_value = it->substr(pos + 1);
					if (check_ints(_value) == -1 || atoi(_value.c_str()) > 65535)
					{
						std::cout << "Error : listen" << std::endl;
						exit(1);
					}
					temp.default_data.insert(std::make_pair("listen", _value));
				}
				else if ((it->find("root ")) != std::string::npos)
				{
					_value = it->substr(pos + 1);
					temp.default_data.insert(std::make_pair("root", _value));
				}
				else if ((it->find("auto_indexing ")) != std::string::npos)
				{
					_value = it->substr(pos + 1);
					temp.default_data.insert(std::make_pair("auto_indexing", _value));
				}
				else if ((it->find("error ")) != std::string::npos)
				{
					_value =  it->substr(pos + 1);
					temp.default_data.insert(std::make_pair("error", _value));
				}
				else if (it->find("server {") != std::string::npos ) { }
				else if (it->find("location ") != std::string::npos)
				{
					std::string directory = it->substr(it->find(" ") + 1);
					it++;
					if (directory.find_first_of("{}") != std::string::npos || directory.find("/") == std::string::npos)
					{
						std::cout << "Error with one of the locations "  << std::endl;
						exit(1);
					}
					std::map<std::string, std::string> tem;
					while (it->find("}") == std::string::npos)
					{
						size_t pos = it->find(" ");
						std::string _key = it->substr(0, pos);
						std::string _value = it->substr(pos + 1);
						// std::cout << _value << std::endl;
						//need to check for cgi and redirections
						std::stringstream  a(_value);
						std::string buff;
						int i = 0;
						if (_key == "root" && _value.find_last_of("/") != _value.length() - 1)
							_value.append("/");
						else if (_key == "cgi" || _key == "redirect" || _key == "allowed_methods")
						{
							// std::cout << " /////////////////   " << _value << std::endl;
							while(getline(a, buff, ' '))
							{
								if (_key == "redirect" && i == 0)
								{
									int a = atoi(buff.c_str());
									if (check_ints(buff) == -1 || a > 399 || a < 300)
									{
										std::cout << "Error : redirect Status" << std::endl;
										exit(1);
									}
								}
								i++;
							}
							if (i != 2 && (_key == "cgi" || _key == "redirect"))
							{
								std::cout << "error in "  << _key << std::endl;
								exit(1);
							}
						}
						
						if (_value.find_first_of("{}") == std::string::npos)
							tem.insert(std::make_pair(_key, _value));
						// for (std::map<std::string, std::string>::iterator oit = tem.begin(); oit != tem.end(); oit++)
						// 	std::cout << oit->first << " " <<  oit->second <<  std::endl;
						it++;
					}
					locations.insert(std::make_pair(directory, tem));
					tem.clear();
					// std::cout << directory << std::endl;
				}
				else {
					std::cout << "Error undefined variable" << std::endl;
					exit(1);
				}
				it++;
			}
			if (locations.size() == 0)
			{
				std::cout << "Please provide at least one location" << std::endl;
			}
			temp.location = locations;
			servers_data.push_back(temp);
		}


		template <typename inputIterator> 
		inputIterator advance_(inputIterator iter) {
			std::string lines;
			std::vector <std::string> temp;

			while (iter->find("};") == std::string::npos)
			{
				// std::cout <<"*" <<  *iter <<  "*" << std::endl;
				temp.push_back(*iter);
				iter++;
			}
			// temp.push_back("};");
			save_data(temp);
 			// std::cout << lines << std::endl;
			return iter;
		}

		void readAndParse()
		{ 
			std::string buff;
			file.open(filename);
			if (file.is_open())
			{
				std::vector <std::string> before_file;
				while(getline(file, buff))
				{
					//first reduce spaces
					buff = reduce(buff, " \t\n\r\v\f");
					// std::cout << "*" << buff << "*" << std::endl;
					before_file.push_back(buff);
					// std::cout << buff << std::endl;
				}
				if ((before_file.end() - 1)->find("};") == std::string::npos || brackets.size() > 0)
				{
					std::cout << "Error in brackets" << std::endl;
					exit(1);
				}
				// std::cout << " size >> "  <<  before_file.size() << std::endl;
				for (std::vector <std::string>::iterator it = before_file.begin(); it != before_file.end(); it++) 
				{
					if (it->find("server {") != std::string::npos)
						it = advance_(it);
					if (it->find("server{") != std::string::npos)
					{
						std::cout << "Error "<< std::endl;
						exit(1);
					}
				}
				// for (std::map<std::string, std::string>::iterator it = servers_data.at(0).default_data.begin(); it !=servers_data.at(0).default_data.end(); it++)
				// 	std::cout << it->first  << " "  << it->second << std::endl;
				// std::cout << "/// * *   "   <<  servers_data.size() << std::endl;
				// std::cout << servers_data.at(0).location.find("/test/urmom")->second["root"];
			}
			else { std::cout << "File Not Found" << std::endl;
				exit(1);
			}
		}

	void mime() { 
		std::ifstream file_mime;
		std::string buff;
		file_mime.open("mime_type.txt");
		if (file_mime.is_open())
		{
			while (getline(file_mime, buff))
			{
				// std::cout << buff << std::endl;
				size_t pos = buff.find_last_of(" ");
				std::string key = buff.substr(0, buff.find(" "));
				std::string value_ = buff.substr(pos + 1);
				mime_type.insert(std::make_pair(key, value_));
			}
		} else {
			std::cout << "mime type is not found" << std::endl;
			exit(1);
		}
	}
	std::vector<data_serv> get_server_data() const { 
		return servers_data;
	}
	~parsing() {}
};



#endif