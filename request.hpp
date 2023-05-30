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
	data_header() : Content_Length(-2) {}
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

	request() : rsize(0) , size(0), chunked_size(-2), end_of_file(false)  { }
	~request() {}

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


	void	save_chunk_improve(std::string &body) {
		
		// std::cout << body <<   " \n THE END  " << std::endl; 
		std::string::iterator first = body.begin();
		std::string::iterator last;
		std::string to_delete = "\r\n";
		if (file_obj.is_open() == false)
		{
			if (file.empty() == true)
			{
				body.insert(0, "\r\n");
				std::string name = time_date();
				file = name + "." + parsing::mime_type.find(d_header.Content_type)->second;
				// std::cout <<  file << std::endl;
			}
			// body.erase(0, body.find(to_delete));
			file_obj.open(file, std::fstream::out | std::fstream::app | std::fstream::binary);
		}
		std::stringstream to_hex;
		while (chunked_size != 0)
		{
			if (chunked_size == -2) // kata3ni badya dyal lbody fih chunked
			{
				first = std::search(body.begin(), body.end(), to_delete.begin(), to_delete.end());

				std::string::iterator te_end =  std::search(first + 1, body.end(), to_delete.begin(), to_delete.end());


				std::string test(first + 2, te_end);
				if (test == "0")
				{
					end_of_file = true;
					d_header.res_status = 200;
				}
				to_hex << test;
				to_hex >> std::hex >> chunked_size;
				body.erase(0, test.length() + 4);
			}
			if (chunked_size >= body.size())
			{
				file_obj << body;
				chunked_size -= body.size();
				body.clear();
				if (chunked_size == 0) chunked_size = -2;
				break ;
			}
			else
			{
				file_obj << body.substr(0, chunked_size);
				body.erase(0, chunked_size);
				chunked_size = -2;
				if (body.size() <= 2 || body.find("\r\n", 2) == std::string::npos)
				{
					break ;
				}
			}
		}
		if (end_of_file == true)
			file_obj.close();
	}

	/* HERE IS THE EMMDDD */
	void save_chunked(std::string &body)
	{
		std::string::iterator first = body.begin();
		std::string::iterator last;
		std::string to_delete = "\r\n";
		if (file_obj.is_open() == false)
		{
			if (file.empty() == true)
			{
				body.insert(0, "\r\n");
				std::string name = time_date();
				file = name + "." + parsing::mime_type.find(d_header.Content_type)->second;
			}
			file_obj.open(file, std::fstream::out | std::fstream::app | std::fstream::binary);
		}
		
		rsize += body.length(); 
		long index = 0;
		std::string temp;
		std::stringstream stream;
		while (rsize > chunked_size && end_of_file == false)
		{
			index  = chunked_size + body.length() - rsize;
			if (index < 0)
			{
				return ;
			}
			temp = copy_until(body.begin() + index + 2, body.end());
			if (temp == "0")
				end_of_file = true;
			stream << temp;
			stream >> std::hex >> chunked_size;
			chunked_size += temp.size() + 4;
			body.erase(index, index + temp.size() + 4);
		}
		file_obj << body;
		file_obj.close();
		if (index == 0 && end_of_file == true)
			d_header.res_status = 200;
	}

	void save_binary(std::string &header)
	{
		if (file.empty() == true)
		{
			std::string name = time_date();
			std::cout <<  "  / * * / */ -" << d_header.Content_type << std::endl;
			file = name + "." + parsing::mime_type.find(d_header.Content_type)->second;
		}
		file_obj.open(file, std::fstream::out | std::fstream::binary | std::fstream::app);
		if (file_obj.is_open())
		{
			size += header.size();
			file_obj << header;
			header.clear();
		}
		file_obj.close();
		if (size == d_header.Content_Length)
		{
			file.clear();
			header.clear();
			end_of_file = true;
			d_header.res_status = 200;
		}
	}

	void parse(std::string &header) 
	{
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
			std::string allowed_char = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=";
			for (std::string::iterator it = d_header.uri.begin(); it != d_header.uri.end(); it++)
				if (allowed_char.find(*it) == std::string::npos)
				{
					d_header.res_status = 400;
					std::cout << "ERRORR " << std::endl;
					return ;
				}
			if (d_header.uri.length() > 2048)
			{
				d_header.res_status = 414;
				return ;
			}
			// still need to check for request entity too large
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
			}
			if (header.find("\r\n\r\n") != std::string::npos)
				header.erase(0, header.find("\r\n\r\n") + 4);
			else {
				d_header.res_status = 431;
				return ;
			}
		}
		if (d_header.transfer_encoding.empty() == false)
		{
			
			if (d_header.transfer_encoding.find("chunked") == std::string::npos 
			|| (d_header.boundary.empty() == false 
			&& d_header.transfer_encoding.find("chunked") == std::string::npos))
			{
				d_header.res_status = 501;
				return ;
			}
		}
		if (d_header.method == "POST")
		{
			if (d_header.transfer_encoding.empty() == true && d_header.Content_Length ==  -2)
			{
				d_header.res_status = 400;
				return;
			}
			else if (d_header.transfer_encoding.empty() == false)
			{
				save_chunk_improve(header);
			}
			else // needs to check for cgi  
			{
				save_binary(header);
			}
		}
		if (d_header.method.find("GET") != std::string::npos)
		{
			end_of_file = true;
			return ;
		}
	}

};
#endif
		