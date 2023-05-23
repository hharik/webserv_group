#ifndef REQUEST_HPP_

#define REQUEST_HPP_

#include	<algorithm>
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
	long rsize;
	int chunked_size;
	int size;
	bool end_of_file;
	std::fstream file_obj;


	public:

	request() : rsize(0) , size(0), chunked_size(0), end_of_file(false)  { }
	~request() {}

	// void	getfilename(std::string::iterator &first, std::string::iterator &last)
	// {
		
	// }

	// void	save_files(std::string &body) 
	// {
	// 	while (body.empty() == true)
	// 	{
	// 		std::string::iterator first = body.begin();
	// 		std::string::iterator last;
	// 		last = std::search(body.begin(), body.end(), d_header.boundary.begin(), d_header.boundary.end());
	// 		if (file_obj.is_open() == false)
	// 		{
	// 			getfilename(first, last);
	// 		}
	// 	}
	// }

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


	std::string copy_until(std::string::iterator first, std::string::iterator last, std::string to_find = "\r\n") {
		std::string temp;
		while (first < last)
		{
			if (*first == '\r' && *(first + 1) && *(first + 1) == '\n')
			{
				break ;
			}
			temp += *first;
			first++;
		}
		return temp;
	}


	void save_chunked(std::string &body)
	{
		// std::cout << body << " \n THE END " << std::endl;
		std::string::iterator first = body.begin();
		std::string::iterator last;
		std::string to_delete = "\r\n";
		if (file_obj.is_open() == false)
		{
			if (file.empty() == true)
			{
				body.insert(0, "\r\n");
				std::string name = time_date();
				file = name + "." + mime_type.find(d_header.Content_type)->second;
				// std::cout <<  file << std::endl;
			}
			// body.erase(0, body.find(to_delete));
			file_obj.open(file, std::fstream::out | std::fstream::app | std::fstream::binary);
		}
		
		rsize += body.length(); 
		long index = 0;
		std::string temp;
		// std::cout << body << std::endl;
		std::stringstream stream;
		while (rsize > chunked_size && end_of_file == false)
		{
			index  = chunked_size + body.length() - rsize;
			if (index < 0)
				exit(122);
			//copy until buff + a + 2 till body.end(), 
			// temp = body.substr(body.begin() + index + 2, body.size());
			temp = copy_until(body.begin() + index + 2, body.end());
			if (temp == "0")
				end_of_file = true;
			stream << temp;
			stream >> std::hex >> chunked_size;
			chunked_size += temp.size() + 4;
			body.erase(index, index + temp.size() + 4);
			// std::cerr << temp << std::endl;
			// exit(1);
		}
		// std::cout << body << std::endl;
		file_obj << body;
		file_obj.close();
		if (index == 0 && end_of_file == true)
			d_header.res_status = 200;
	// 	if (chunked_size == 0)
		// 	{
		// 		std::cout << "heeel " << std::endl;
		// 		std::stringstream stream;
		// 		std::string::iterator fff = std::search(body.begin(), body.end(), to_delete.begin(), to_delete.end());
		// 		std::string::iterator endd = std::search(fff + 1, body.end(), to_delete.begin(), to_delete.end());
		// 		std::string tes = std::string(fff, endd);
		// 		// std::cout <<"*"<<  tes << " - * -* " << std::endl;
		// 		body.erase(body.begin(), endd);
		// 		stream << tes;
		// 		stream >> std::hex >> chunked_size;
		// 		chunked_size -= (tes.size());
		// 	}
		// 	// std::cout << body  << std::endl;
		// 	if (chunked_size > 0)
		// 	{
		// 		if (body.length() >= chunked_size)
		// 		{
		// 			std::cout << "heree iss  " << std::endl;
		// 			file_obj << body.substr(0, chunked_size);
		// 			body.erase(chunked_size);
		// 			chunked_size = 0;
		// 		}
		// 		else if (body.length() < chunked_size)
		// 		{
		// 			std::cout << "heeel " << std::endl;
		// 			file_obj << body;
		// 			chunked_size -= body.length();
		// 			body.clear();
		// 			break ;
		// 		}
		// 	}
		// }
		// if (chunked_size > 0)
	
		// size_t pos = 0;
		// while ((first = std::search(first,body.end(), to_delete.begin(), to_delete.end())) != last)
		// {
		// 	last = std::search(first + 1, body.end(), to_delete.begin(), to_delete.end());
		// 	std::cout << std::string(first, last);
		// 	body.erase(first,last);
		// 	first ++;
		// 	// first += 2;
		// 	// if (last != body.end())
		// 	// {
		// 	// 	body.erase(first, last);
		// 	// 	first += 2;
		// 	// }
		// 	// if (end_pos != std::string::npos)
		// 	// {
		// 	// 	body.erase(pos, end_pos);
		// 	// }
		// 	// pos += 2;
		// std::cout << body << std::endl;
		// }
	
	}

	void save_binary(std::string header)
	{
		// std::cout <<"  content lemgf  : " <<  d_header.Content_Length << std::endl;
		if (file.empty() == true)
		{
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
		// std::cout << header  << "\n THE END" << std::endl;
		// std::cout << "THE END "<< std::endl;
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
		// std::cout << " THE END" << std::endl;
		if (d_header.method == "POST")
		{
			if (d_header.Content_Length == 0)
			{
				d_header.res_status = 400;
			}
			else if (d_header.transfer_encoding.empty() == false)
			{
				if (d_header.transfer_encoding.find("chunked") == std::string::npos || (d_header.boundary.empty() == false && d_header.transfer_encoding.find("chunked") == std::string::npos))
				{
					d_header.res_status = 501;
				}
				else 
				{
					
					save_chunked(header);
				}
				// save_files(header);
			}
			else if (d_header.boundary.empty() == true)
			{
				save_binary(header);
			}
		}
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