#ifndef REQUEST_HPP_

#define REQUEST_HPP_


#include "parsing.hpp"

struct data_header {
	std::map<std::string, std::string> header_data;
	std::string method;
	std::string uri;
	std::string http_version;
	int res_status;
};

class request {
	data_header d_header;
	public:
	request() { }
	~request() {}
	void parse(std::string header) {
		std::cout <<  header  << std::endl;
		std::string buff;
		std::string tem;
		std::vector<std::string> _v;
		std::string token;
		std::stringstream aa(header);
		getline(aa, buff, '\n');
		while (getline(aa, tem, '\n'))
		{
			_v.push_back(tem);
		}
		// std::cout << " _ > "  << buff << std::endl;
		std::stringstream bb(buff);
		int i = 0;
		while (getline(bb, token, ' '))
		{
			std::cout << token << std::endl;
			if (i == 0)
				d_header.method = token;
			else if (i == 1)
				d_header.uri = token;
			else if (i == 2)
				d_header.http_version = token;
			i++;
		}
		if (i > 3)
			d_header.res_status = 400;
		// std::cout <<  " * " <<  d_header.res_status << std::endl;
		for (std::vector<std::string>::iterator it = _v.begin(); it != _v.end(); it++)
		{
			size_t pos = it->find(":");
			if (pos != std::string::npos)
				d_header.header_data.insert(std::make_pair(it->substr(0, pos), it->substr(pos + 1)));
		}
		// for (std::map<std::string,std::string>::iterator it = d_header.header_data.begin(); it != d_header.header_data.end(); it++)
		// 	std::cout << it->first << " " << it->second << std::endl;
	}
};








#endif