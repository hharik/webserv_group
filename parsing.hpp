#ifndef PARSING_HPP_
#define PARSING_HPP_
#include <iostream>
#include <fstream>
#include <ostream>
#include <vector>
#include <stack>
#include <map>

struct data_serv {
	std::map<std::string, std::map<std::string, std::string> > location;
	std::map <std::string, std::string> default_data;
	// data_serv(const data_serv & other): location(other.location), default_data(other.default_data) { }
	// data_serv(std::map<std::string, std::string> da, std::map<std::string, std::map<std::string, std::string> > loc) 
	// : default_data(da), location(loc) {}
};

class parsing
{
	private:
		std::vector <std::string> before_file; 
		std::string filename;
	
		std::stack <std::string> brackets;
		std::ifstream file;
		std::vector<data_serv> servers_data;
	public:
		parsing(std::string name) : before_file(), filename(name), file(), servers_data() {
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
		std::string reduce(std::string buff, std::string whitespaces) {
			buff = trim(buff, " \t\r\v\f\n");
			std::size_t beginSpace = buff.find_first_of(whitespaces);
			std::size_t endSpace;
			while (beginSpace != std::string::npos)
			{
				endSpace = buff.find_first_not_of(whitespaces, beginSpace);
				std::size_t range = endSpace - beginSpace;
				buff.replace(beginSpace, range, " ");
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
				if (brackets.size() == 0)
				{
					std::cout << "Unclosed bracket" << std::endl;
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
			for (; it != server.end(); it++)
			{
				// save default data
				pos = it->find(" ");
				// std::cout << "*"<<  *it << "*num " << std::endl;
				if ((it->find("server_name")) != std::string::npos)
					temp.default_data.insert(std::make_pair("server_name", it->substr(pos + 1)));
				else if ((it->find("max_body_size")) != std::string::npos)
					temp.default_data.insert(std::make_pair("max_body_size", it->substr(pos + 1)));
				else if ((it->find("listen")) != std::string::npos)
					temp.default_data.insert(std::make_pair("listen", it->substr(pos + 1)));
				else if ((it->find("root")) != std::string::npos)
					temp.default_data.insert(std::make_pair("root", it->substr(pos + 1)));
				else if ((it->find("auto_indexing")) != std::string::npos)
					temp.default_data.insert(std::make_pair("auto_indexing", it->substr(pos + 1)));
				else if ((it->find("error")) != std::string::npos)
					temp.default_data.insert(std::make_pair("error", it->substr(pos + 1)));
				else if (it->find("server {") != std::string::npos)
					it++;
				else if (it->find("location") != std::string::npos)
				{
					std::string directory = it->substr(it->find(" ") + 1);
					it++;
					std::map<std::string, std::string> tem;
					while (it->find("}") == std::string::npos)
					{
						size_t pos = it->find(" ");
						std::string _key = it->substr(0, pos);
						std::string _value = it->substr(pos + 1);
						//need to check for cgi and redirections
						tem.insert(std::make_pair(_key, _value));
						it++;
					}
					locations.insert(std::make_pair(directory, tem));
					tem.clear();
					// std::cout << directory << std::endl;
				}
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
			temp.push_back("};");
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
				while(getline(file, buff))
				{
					//first reduce spaces
					buff = reduce(buff, " \t\n\r\v\f");
					// std::cout << "*" << buff << "*" << std::endl;
					before_file.push_back(buff);
				}
				if ((before_file.end() - 1)->find("};") == std::string::npos)
				{
					std::cout << "Error in brackets" << std::endl;
					exit(1);
				}
				for (std::vector <std::string>::iterator it = before_file.begin(); it != before_file.end(); it++) 
				{
					if (it->find("server {") != std::string::npos)
						it = advance_(it);
				}
			}
			else { std::cout << "File Not Found" << std::endl;
				exit(1);
			}
		}
		
	~parsing() {}
};



#endif