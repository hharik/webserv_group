#ifndef REQUEST_HPP_

#define REQUEST_HPP_


#include "parsing.hpp"



struct data_header {
	std::string Content_type;
	std::string Host;
	int Content_Length;
	std::string transfer_encoding;
	std::string method;
	std::string uri;
	std::string http_version;

	std::string boundary;
	int res_status;
	void print() {
		std::cout << " h" << Content_type <<  " " << Host << " " << Content_Length << " " << transfer_encoding << " " << method << std::endl;
	}
	bool empty() {
		if (Host.empty() == true && transfer_encoding.empty() == true && Content_type.empty() == true)
			return true;
		return false;
	}
};

class request {

	data_header d_header;
	std::string file;
	int size;
	std::fstream file_obj;


	public:

	request() : size(0)  { }
	~request() {}


	void	save_files(std::string body) 
	{
		// std::cout << body << "\n THE END "    << std::endl;
		std::size_t pos = 0;
		// while ()
		// {

		// }
		// 	exit(1);
			// pos = body.find(d_header.boundary, pos +1);
		// }

















		// std::vector<std::string> files;
		// size_t pos = 0;
		// while ((pos = body.find(d_header.boundary, pos)) !=  std::string::npos)
		// {
		// 	pos += d_header.boundary.length();
		// 	size_t new_pos = body.find(d_header.boundary, pos);
		// 	std::cout << body.substr(pos, new_pos - pos);
		// }
	}
	std::string time_date()
	{
			time_t curr_time;
			tm * curr_tm;
			char date_string[100];
			char time_string[100];

			time(&curr_time);
			curr_tm = localtime(&curr_time);

			strftime(date_string, 50, "%B-%d-%Y-%T", curr_tm);
			return std::string(date_string);
		
	}
	void save_binary(std::string header)
	{
		// std::cout <<"  content lemgf  : " <<  d_header.Content_Length << std::endl;
		if (file.empty() == true)
		{
			// std::cout <<  d_header.Host  << " /                    **********  \n";
			std::string name = time_date();
			std::cout <<  "  / * * / */ -" << d_header.Content_type << std::endl;
			file = name + "." + mime_type.find(d_header.Content_type)->second;
		}
		std::cout << " file to open > "  << file << std::endl;
		file_obj.open(file, std::fstream::out | std::fstream::binary | std::fstream::app);
		if (file_obj.is_open())
		{
			size += header.size();
			file_obj << header;
		}
		file_obj.close();
		if (d_header.Content_Length == size)
		{
			file.clear();
			d_header.res_status = 200;
		}
	}

	void parse(std::string header) 
	{
		// std::cout << header << std::endl;
		std::string tes;
		if (d_header.Host.empty() == true)
		{
			// std::cout << header << std::endl; 
			std::string first_line, buffer, token;
			std::stringstream ss(header);
			getline(ss, first_line, '\n');
			std::stringstream aa(first_line);
			for (int i = 0; getline(aa, token, ' '); i++)
			{
				if (i > 2)
				{
					d_header.res_status = 400;
					return ;
				}
				else if (i == 0)
					d_header.method = token;
				else if (i == 1)
					d_header.uri = token;
				else if (i == 2)
					d_header.http_version = token;
			}
			while (getline(ss, buffer, '\n'))
			{
				size_t pos = buffer.find(":");
				if (buffer.find("Content-Length:") != std::string::npos)
				{
					d_header.Content_Length = atoi((buffer.substr(pos + 2)).c_str());
					// d_header.Content_Length.pop_back();
					// std::cout << d_header.Content_Length << std::endl;
				}
				if (buffer.find("Content-Type:") != std::string::npos)
				{
					d_header.Content_type = buffer.substr(pos + 2);
					d_header.Content_type.pop_back();
					// std::cout << " \n\n\n" <<  buffer <<  " just tryiubg /*/*/" << std::endl;
					if (d_header.Content_type.find("multipart/form-data") != std::string::npos)
					{
						d_header.boundary = buffer.substr(buffer.find_last_of("boundary=") + 1);
						d_header.boundary.pop_back();
					}
					// std::cout << d_header.boundary << std::endl;
					// std::cout << d_header.Content_type	<< 	std::endl;
				}
				if (buffer.find("Host:") != std::string::npos)
				{
					d_header.Host = buffer.substr(pos + 2);
					d_header.Host.pop_back();
					// std::cout << d_header.Host	<< std::endl;
				}
				if (buffer.find("Transfer-Encoding:") != std::string::npos)
				{
					d_header.transfer_encoding = (buffer.substr(pos + 1));
				}
				if (buffer.size() == 1 && buffer == "\r")
				{
					break ;
				}
				// std::cout << buffer  <<  std::endl;
			}
			header.erase(0, header.find("\r\n\r\n") + 4);
		}
	// std::cout << header << std::endl;
		if (d_header.boundary.empty() == true)
		{
			save_binary(header);
		// 	std::string filename = time_date();
		// 	std::string extension = "." + mime_type.find(d_header.Content_type)->second;
		// 	std::cout << " file to open > "  << filename + extension << std::endl;
		// 	file_obj.open(filename + extension, std::ifstream::binary);
        // }
        }
		// std::cout <<  "here is the incremented  " <<  size << std::endl;
		// std::cout << "here is the total content_len " << d_header.Content_Length << std::endl;
		// 	if (file_obj.is_open())
		// 	{

		// 	}
		// }



			// save_files(header);
		// else
		// 	save_binary(header);
		// std::cout << header << std::endl;
		// exit(1);
			// d_header.print();
	// header =  &header[header.find("\r\n") + 4];
	// std::cout << "header \n" << header << std::endl;


	// if ((pos = header.find("Host:")) != std::string::npos)
	// {
	// 	d_header.Host = header.substr(pos + std::string("Host:").length() , header.find("\n"));
	// }
	// if ((pos = header.find("Content-Type:")) != std::string::npos)
	// 	d_header.Content_type = header.substr((pos + std::string("Content-Type:").length()) , header.find("\n", pos));
	// std::cout << d_header.Host << std::endl;
	// std::cout << d_header.Content_type << std::endl;

};






#endif
	// std::vector <std::string> headers, body;
	// while (getline(ss, token, '\n'))
	// {
	// 	headers.push_back(token);
	// }
	// int count = 0;
	// std::vector <std::string>::iterator it;
	// for (it = headers.begin(); it != headers.end(); it++)
	// {
	// 	std::cout << *it << std::endl;
	// 	size_t pos = it->find(":");
	// 	if (it->find("Host") != std::string::npos)
	// 		d_header.Host = it->substr(pos + 1);
	// 	else if (it->find("Content-Type:") != std::string::npos && d_header.Content_type.empty())
	// 	{
	// 		d_header.Content_type = it->substr(pos + 1);
	// 		if (d_header.Content_type.find("multipart/form-data") != std::string::npos)
	// 		{
	// 			size_t pos1 = d_header.Content_type.find_last_of("-");
	// 			if (pos1 != std::string::npos)
	// 				d_header.boundary = it->substr(pos1);
	// 		}
	// 	}
	// 	else if (it->find("Content-Length:") != std::string::npos && d_header.Content_Length.empty())
	// 		d_header.Content_Length = it->substr(pos + 1);
	// 	else if (it->find("Transfer-Encoding:") != std::string::npos && d_header.transfer_encoding.empty())
	// 		d_header.transfer_encoding = it->substr(pos + 1);
		
		
		
		
		
		
		
};