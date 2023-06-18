/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hharik <hharik@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 13:59:47 by hharik            #+#    #+#             */
/*   Updated: 2023/06/18 11:31:11 by hharik           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parsing.hpp"

int parsing::check_ints(std::string str) 
{
	for (std::string::iterator it = str.begin(); it != str.end() ; it++)
	{
		if ((!isdigit(*it)))
		{
			if (*it == '-' || *it == '+')
			{
				if ( !isdigit(*(it + 1)))
					return -1;
			}
			else
				return -1;
		}
	}
	return 0;
}
	
parsing::parsing(std::string name) : filename(name), end_brackets(), file() ,servers_data()
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

int parsing::save_data(std::vector <std::string> server)
{
	data_serv temp;
	int status = 0;
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
				status = -1;break ;
			}
			if ((point = _value.find(":")) == std::string::npos)
			{
				std::cout << "Server_name not provided and doesnt have a specific port" << std::endl;
				status =  -1;break ;
			}
			temp.port = it->substr(it->find(":") + 1);
			temp.server_name = it->substr(pos + 1, point);
			if (check_ints(temp.port) == -1)
			{
				std::cout << "Error : port specified is not integer" << std::endl;
				status =  -1;break ;
			}

		}
		else if ((it->find("max_body_size ")) != std::string::npos)
		{
			_value = it->substr(pos + 1);
			if (_value.find(" ") != std::string::npos)
			{
				std::cout << "error " <<  std::endl;
				status =  -1;break ;
			}

			if (check_ints(_value) == -1)
			{
				std::cout << "Error : Max body size isnt int" << std::endl;
				status =  -1;			break ;
			}
			temp.max_body_size = std::atoi(_value.c_str());
			if (temp.max_body_size < 0)
				temp.max_body_size = 0;
		}
		else if ((it->find("index ")) != std::string::npos)
		{
			temp.index = it->substr(pos +  1);
			if (temp.index.find(" ") != std::string::npos)
			{
				std::cout << "Error in default index " << std::endl;
				status =  -1;	break ;
			}
			if (temp.index.find("/") == 0)
			{
				temp.index.erase(0, 1);
			}
		}
		else if (it->find("cgi_timeout ") != std::string::npos)
		{
			_value = it->substr(pos + 1);
			if ((_value.find(" ") != std::string::npos))
			{
				std::cout << "error " << std::endl;
				status =  -1;	break ;
			}
			else if (_value != "on" && _value != "off")
			{
				std::cout << "error in cgi_timeout" << std::endl;
				status =  -1;	break ;
			}
			temp.cgi_mode = _value;
		}
		else if ((it->find("root ")) != std::string::npos)
		{
			temp.root_dir = it->substr(pos + 1);
			if (temp.root_dir.find(" ") != std::string::npos)
			{
				std::cout << "error " << std::endl;
				status =  -1; break ;
			}
			if (temp.root_dir.find_last_of("/") != temp.root_dir.length() - 1)
			{
				temp.root_dir.append("/");
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
				status =  -1;
				break ;

			}
			if (check_ints(status) == -1) {
				std::cout << "Error, please specify error status" << std::endl;
				status =  -1;
				break ;
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
				status =  -1;
				break ;
			}
			if (directory.find_first_of("/") != 0)
			{
				std::cout << "error, please add a slash at the first of location name" << std::endl;
				status =  -1;
				break ;
			}
			it++;
			if (directory.find_first_of("{}") != std::string::npos || directory.find("/") == std::string::npos)
			{
				std::cout << "Error with one of the locations "  << std::endl;
				status =  -1;
				break ;
			}
			std::map<std::string, std::string> tem;
			while (it->find("}") == std::string::npos )
			{
				size_t pos = it->find(" ");
				std::string _key = it->substr(0, pos);
				if (pos == std::string::npos && _key != "{")
				{
					std::cout << _key << std::endl;
					std::cout << "Error with one of the locations " << std::endl;
					status =  -1;
				}
				std::string _value = it->substr(pos + 1);
				std::stringstream  a(_value);
				std::string buff;
				int i = 0;
				if (_key == "root" || _key == "upload")
				{
					if (_value.find(" ") != std::string::npos)
					{
						std::cout << "error" << std::endl;
						status =  -1;
					}
					if (_value.find_last_of("/") != _value.length() - 1)
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
								status =  -1;
							}
						}
						if (_key == "cgi" && i == 0)
						{
							if (buff.find(".") != 0)
							{
								std::cout << "Error add . in the cgi extension!" << std::endl;
								status =  -1;
							}
						}
						i++;
					}
					if ( i != 2  && (_key == "cgi" || _key == "redirect"))
					{
						std::cout << "error in "  << _key << std::endl;
						status =  -1;				
					}
				}
				else if (_key == "error" || _key == "max_body_size" || _key == "listen" || _key == "cgi_timeout") {
					std::cout << "Can't specify variable "  << _key << " pages in location " << std::endl;
					status =  -1;
				}
				else if ((_key == "index")) 
				{
					if (_value.find(" ") != std::string::npos) 
					{
						std::cout << "error " << std::endl;
						status =  -1;
					}
					if (_value.find("/") == 0)
					{
						_value.erase(0, 1);
					}
				}
				else if ((_key == "auto_indexing"))
				{
					if ((_value.find(" ") != std::string::npos))
					{
						std::cout << "error " << std::endl;
						status =  -1;
					}
					else if (_value != "on" && _value != "off")
					{
						std::cout << "error in auto_indexing" << std::endl;
						status =  -1;
					}
				}
				else if (_key != "{" && _key != "}" && _key.empty() == false)
				{
					std::cout << "error undefined variable in location" << std::endl;
					status = -1;
				}
				if (_value.find_first_of("{}") == std::string::npos)
				{
					if (_key == "cgi")
					{
						size_t po = _value.find(" ");
						_key += " " + _value.substr(0, po);
						_value.erase(0,  _value.find(" ") + 1);
					}
					tem.insert(std::make_pair(_key, _value));
				}
				else if (_value.find("};") != std::string::npos)
				{
					std::cout << "ERROR " << std::endl;
					status =  -1;
				}

				it++;
			}
			locations.insert(std::make_pair(directory, tem));
			tem.clear();
		}
		else {
			std::cout << *it << std::endl;
			std::cout << "Error undefined variable" << std::endl;
			status =  -1;
		}
		it++;
	}
	if (temp.root_dir.empty() == true || temp.index.empty() == true) {
		std::cout << "please, provide default index or root directory" << std::endl;
		status =  -1;
	}
	if (locations.size() == 0)
	{
		std::cout << "Please provide at least one location" << std::endl;
		status =  -1;
	}
	temp.location = locations;
	if (status != -1)
		servers_data.push_back(temp);
	else
	{
		temp.location.clear();
		temp.cgi_mode.clear();
		temp.errors.clear();
		temp.root_dir.clear();
		temp.port.clear();
		temp.server_name.clear();
		temp.index.clear();
	}
	return 0;
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
			if (buff.empty() == false)
			{
				buff = reduce(buff, " \t\n\r\v\f");
				tess += buff;
				before_file.push_back(buff);
			}
		}
		if (before_file.size() == 0)
		{
			std::cout << "CONFIG EMPTY" << std::endl;
			exit(1);
		}
		if ((before_file.end() - 1)->find("};") == std::string::npos || brackets.size() > 0 || end_brackets.size() > 0)
		{
			std::cout << "Error in brackets" << std::endl;
			exit(1);
		}
		int i = 0;
		std::vector<std::string> temp;
		for (std::vector <std::string>::iterator it = before_file.begin(); it != before_file.end(); it++)
		{
			if (it->find("server {") != std::string::npos)
				i = INSERV;
			if (it->find("};") != std::string::npos )
			{
				i = OUTSERV;
				save_data(temp);
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
			if (encounted.find(it->port) != encounted.end())
			{
				std::cout << "error please use differnt port" << std::endl;
				exit(1);
			}
			encounted[it->port] = true;
		}
		before_file.clear();
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