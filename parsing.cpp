#include "parsing.hpp"
#include <iterator>
#include <sys/types.h>
#include <sys/stat.h>

int parsing::check_ints(std::string str) 
{
	for (std::string::iterator it = str.begin(); it != str.end() ; it++)
	{
		if (!isdigit(*it))
			return -1;
	}
	return 0;
}
	
parsing::parsing(std::string name) : filename(name), file(), servers_data(), end_brackets() 
{
}

std::string parsing::trim(std::string line, std::string whitespaces)
{
	std::size_t strbegin = line.find_first_not_of(whitespaces);
	if (strbegin == std::string::npos)
		return " ";
	std::size_t strend =  line.find_last_not_of(whitespaces);
	std::size_t strRange = strend - strbegin + 1;
	return line.substr(strbegin, strRange);
}

int parsing::is_file_or_directory(const char *str)
{
	struct stat statbuff;
	if (stat(str, &statbuff) != 0)
		return -1;
	if (S_ISDIR(statbuff.st_mode) != 0)
		return 1;
	return 0;
}


std::string parsing::reduce(std::string buff, std::string whitespaces, std::string to_replace = " ")
{
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
	if ((che  = buff.find("server {")) != std::string::npos)
	{
		// std::cout << buff << std::endl;
		end_brackets.push("{;");
	}
	if ((che = buff.find("};")) != std::string::npos)
	{
		if (end_brackets.size() == 0)
		{
			std::cout << "ERROR " << std::endl;
			exit(1);
		}
		else 
			end_brackets.pop();
	}
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

void parsing::save_data(std::vector <std::string> server)
{
	data_serv temp;
	std::map<std::string, std::map<std::string, std::string> > locations;
	size_t pos;

	std::vector <std::string>::iterator it = server.begin();

	while (it != server.end())
	{
		// save default data
		std::string _value;
		pos = it->find(" ");
		if ((it->find("listen ")) != std::string::npos && temp.server_name.empty() == true)
		{
			size_t point = 0;
	
			_value = it->substr(pos + 1);
			if (_value.find(" ") != std::string::npos)
			{
				std::cout << "error" << std::endl;
				exit(1);
			}
			if ((point = _value.find(":")) == std::string::npos)
			{
				std::cout << "Server_name not provided and doesnt have a specific port" << std::endl;
				exit(1);
			}
			temp.port = it->substr(it->find(":") + 1);
			temp.server_name = it->substr(pos + 1, point);
			// std::cout << temp.server_name << " EHEE " << temp.port << std::endl;
			// std::cout << " //-/ -/  " <<  it->substr(pos + 1) << std::endl;
			// temp.default_data.insert(std::make_pair("server_name", _value));
		}
		else if ((it->find("max_body_size ")) != std::string::npos)
		{
			_value = it->substr(pos + 1);
			if (_value.find(" ") != std::string::npos)
			{
				std::cout << "error " <<  std::endl;
				exit(EXIT_FAILURE);
			}

			if (check_ints(_value) == -1)
			{
				std::cout << "Error : Max body size isnt int" << std::endl;
				exit(1);
			}
			temp.max_body_size =  std::atoi(_value.c_str());
			// std::cout << temp.max_body_size << std::endl;
			// temp.default_data.insert(std::make_pair("max_body_size", _value));
		}
		else if ((it->find("root ")) != std::string::npos)
		{
			temp.root_dir = it->substr(pos + 1);
			if (temp.root_dir.find(" ") != std::string::npos)
			{
				std::cout << "error " << std::endl;
				exit(EXIT_FAILURE);
			}
			if (is_file_or_directory(temp.root_dir.c_str()) != 1 && temp.root_dir.find_last_of("/") != temp.root_dir.length() - 1)
			{
				temp.root_dir.append("/");
			}
			// std::cout << temp.root_dir << std::endl;
			// temp.default_data.insert(std::make_pair("root", _value));
		}
		else if ((it->find("auto_indexing ")) != std::string::npos)
		{
			if ((temp.auto_index = it->substr(pos + 1)).find(" ") != std::string::npos){
				std::cout << "error in auto_indexing " << std::endl;
				exit(EXIT_FAILURE);
			}
			else if (temp.auto_index != "on" && temp.auto_index != "off")
			{
				std::cout << "error in auto_indexing " << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		else if ((it->find("error ")) != std::string::npos)
		{
			_value = it->substr(pos + 1);
			std::string status = _value.substr(0, _value.find(" "));
			std::string path = _value.substr(_value.find(" ") + 1);
			if (path.find(" ") != std::string::npos)
			{
				std::cout << "error "<< std::endl;
				exit(EXIT_FAILURE);
			}
			if (check_ints(status) == -1) {
				std::cout << "Error, please specify error status" << std::endl;
				exit(1);
			}
			temp.errors.insert(std::make_pair(std::atoi(status.c_str()), path));
			// // temp.default_data.insert(std::make_pair("error", _value));
		}
		else if (it->find("server {") != std::string::npos ) { }
		else if (it->find("location ") != std::string::npos)
		{
			std::string directory; 
			if ((directory = it->substr(it->find(" ") + 1)).find(" ") != std::string::npos)
			{
				std::cout << "error space " << std::endl;
				exit(EXIT_FAILURE);
			}
			if (directory.find_first_of("/") != 0)
			{
				std::cout << "error, please add a slash at the first of location name" << std::endl;
				exit(1);
			}
			it++;
			if (directory.find_first_of("{}") != std::string::npos || directory.find("/") == std::string::npos)
			{
				std::cout << "Error with one of the locations "  << std::endl;
				exit(1);
			}
			std::map<std::string, std::string> tem;
			while (it->find("}") == std::string::npos )
			{
				size_t pos = it->find(" ");
				std::string _key = it->substr(0, pos);
				std::string _value = it->substr(pos + 1);
				std::stringstream  a(_value);
				std::string buff;
				int i = 0;
				if (_key == "root" || _key == "upload")
				{
					if (_value.find(" ") != std::string::npos)
					{
						std::cout << "error" << std::endl;
						exit(1);
					}
					if (is_file_or_directory(_value.c_str()) != 1)
					{
						_value.append("/");
					}
				}
				else if (_key == "cgi" || _key == "redirect" || _key == "allowed_methods")
				{
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
					if ( i != 2  && (_key == "cgi" || _key == "redirect"))
					{
						std::cout << "error in "  << _key << std::endl;
						exit(1);
					}
				}
				else if (_key == "error") {
					std::cout << "Can't specify error pages in location " << std::endl;
					exit(1);
				}
				else if ((_key == "index") && (_value.find(" ") != std::string::npos)) { 
					std::cout << "error " << std::endl;
					exit(EXIT_FAILURE);
				}
				else if ((_key == "auto_indexing"))
				{
					if ((_value.find(" ") != std::string::npos))
					{
						std::cout << "error " << std::endl;
						exit(EXIT_FAILURE);
					}
					else if (_value != "on" && _value != "off")
					{
						std::cout << "error in auto_indexing" << std::endl;
						exit(1);
					}
				}
				if (_value.find_first_of("{}") == std::string::npos)
				{
					if (_key == "cgi")
					{
						size_t po = _value.find(" ");
						_key += " " + _value.substr(0, po);
						_value.erase(0,  _value.find(" "));
					}
					tem.insert(std::make_pair(_key, _value));
				}
				else if (_value.find("};") != std::string::npos)
				{
					std::cout << "ERROR " << std::endl;
					exit(1);
				}
				it++;
			}
			// for(std::map<std::string,std::string>::iterator it = tem.begin(); it != tem.end(); it++)
			// {
			// 	std::cout << it->first << "* *" << it->second  << "* "<< std::endl;
			// }
			// std::cout << "THEE END " << std::endl;
			// exit(1);
			locations.insert(std::make_pair(directory, tem));
			tem.clear();
			// std::cout << directory << std::endl;
		}
		else {
			std::cout << *it << std::endl;
			std::cout << "Error undefined variable" << std::endl;
			exit(1);
		}
		it++;
	}
	if (locations.size() == 0)
	{
		std::cout << "Please provide at least one location" << std::endl;
		exit(1);
	}
	temp.location = locations;
	servers_data.push_back(temp);
}


void parsing::readAndParse()
{ 
	std::string buff;
	file.open(filename);
	if (file.is_open())
	{
		std::string tess;
		std::vector <std::string> before_file;
		while(getline(file, buff))
		{
			//first reduce spaces
			buff = reduce(buff, " \t\n\r\v\f");
			// std::cout << "*" << buff << "*" << std::endl;
			tess += buff;
			before_file.push_back(buff);
			// std::cout << buff << std::endl;
		}
		if ((before_file.end() - 1)->find("};") == std::string::npos || brackets.size() > 0 || end_brackets.size() > 0)
		{
			std::cout << "Error in brackets" << std::endl;
			exit(1);
		}
		int i = 0;
		std::vector<std::string> temp;
		size_t pos = 0;
		for (std::vector <std::string>::iterator it = before_file.begin(); it != before_file.end(); it++)
		{
			if (it->find("server {") != std::string::npos)
				i = INSERV;
			if (it->find("};") != std::string::npos )
			{
				i = OUTSERV;
				// std::cout << "helloo world "<< std::endl;
				save_data(temp);
				// std::cout << "\n\n\n\n\nhello world !!!!" << std::endl;
				temp.clear();
			}
			if (i == INSERV)
			{
				temp.push_back(*it);
			}
		}
		std::map<std::string, bool> encounted;
		for (std::vector<data_serv>::iterator it = servers_data.begin(); it != servers_data.end(); it++)
		{
			// std::cout << it->port << std::endl;
			if (encounted.find(it->port) != encounted.end())
			{
				std::cout << "error please use differnt port" << std::endl;
				exit(1);
			}
			encounted[it->port] = true;
		}
	}
	else { std::cout << "File Not Found" << std::endl;
		exit(1);
	}
}

void parsing::mime() 
{
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
			mime_types_ay.insert(std::make_pair(value_, key));
		}
	} else {
		std::cout << "mime type is not found" << std::endl;
		exit(1);
	}
}

parsing::~parsing()
{
	
}