

#include "client.hpp"
#include <string>


request::request( const data_serv *dptr, data_header *hptr ) : server_data(dptr), d_header(hptr), rsize(0) , size(0), chunked_size(-2), end_of_file(false)
{
}

request::~request()
{
	
}


void request::read_body(std::string &body) { 
	size += body.size();
	body.clear();
	if (size >= d_header->Content_Length)
	{
		/* end of the file */
		end_of_file = true;
		d_header->res_status = 200;
		return ;
	}
}

std::string request::time_date()
{
	/* Date: Wed, 11 May 2023 12:00:00 GMT\r\n  */
		time_t curr_time;
		tm * curr_tm;
		char date_string[100];
		char time_string[100];

		time(&curr_time);
		curr_tm = localtime(&curr_time);

		strftime(date_string, 50, "%B-%d-%Y-%T", curr_tm);
		return std::string(date_string);
	
}




void	request::save_chunk_improve(std::string &body) 
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
			file = dir_to_upload +  name + "." + parsing::mime_type.find(d_header->Content_type)->second;
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
				d_header->res_status = 201;
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
		file_obj.flush();
	}
	if (end_of_file == true)
		file_obj.close();
}

void request::save_binary(std::string &header)
{

	if (d_header->file.empty() == true)
	{
		std::string name = time_date();
		std::cout <<  "  / * * / */ -" << d_header->Content_type << std::endl;
		// exit(1);
		d_header->file = dir_to_upload + name + "." + parsing::mime_type.find(d_header->Content_type)->second;
	}
	if (file_obj.is_open() == false)
	{
		file_obj.open(d_header->file, std::fstream::out | std::fstream::binary | std::fstream::app);
	}
	if (file_obj.is_open())
	{
		size += header.size();
		file_obj << header;
		header.clear();
	}
	// file_obj.close();
	if (size == d_header->Content_Length)
	{
		file.clear();
		header.clear();
		end_of_file = true;
		d_header->res_status = 201;
		size = 0;
		file_obj.close();
	}
}

void request::parse(std::string &header) 
{
	std::string tes;
	if (d_header->Host.empty() == true)
	{
		std::string first_line, buffer, token;
		std::stringstream ss(header);
		getline(ss, first_line, '\n');
		std::stringstream aa(first_line);
		for (int i = 0; getline(aa, token, ' '); i++)
		{
			if (i > 2)
			{
				d_header->res_status = 400;
				return ;
			}
			else if (i == 0)
				d_header->method = token;
			else if (i == 1)
				d_header->uri = token;
			else if (i == 2)
				d_header->http_version = token;
		}
		std::string allowed_char = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-._~:/?#[]@!$&'()*+,;=";
		for (std::string::iterator it = d_header->uri.begin(); it != d_header->uri.end(); it++)
			if (allowed_char.find(*it) == std::string::npos)
			{
				d_header->res_status = 400;
				std::cout << "ERRORR " << std::endl;
				return ;
			}
		if (d_header->uri.length() > 2048)
		{
			d_header->res_status = 414;
			return ;
		}
		// still need to check for request entity too large
		while (getline(ss, buffer, '\n'))
		{
			size_t pos = buffer.find(":");
			if (buffer.find("Content-Length:") != std::string::npos)
			{
				d_header->Content_Length = atoi((buffer.substr(pos + 2)).c_str());
				// d_header.Content_Length.pop_back();
				// std::cout << d_header.Content_Length << std::endl;
			}
			if (buffer.find("Content-Type:") != std::string::npos)
			{
				d_header->Content_type = buffer.substr(pos + 2);
				d_header->Content_type.pop_back();
				// std::cout << " \n\n\n" <<  buffer <<  " just tryiubg /*/*/" << std::endl;
				if (d_header->Content_type.find("multipart/form-data") != std::string::npos)
				{
					d_header->boundary = buffer.substr(buffer.find_last_of("boundary=") + 1);
					d_header->boundary.pop_back();
				}
				// std::cout << d_header.boundary << std::endl;
				// std::cout << d_header.Content_type	<< 	std::endl;
			}
			if (buffer.find("Host:") != std::string::npos)
			{
				d_header->Host = buffer.substr(pos + 2);
				d_header->Host.pop_back();
				// std::cout << d_header.Host	<< std::endl;
			}
			if (buffer.find("Transfer-Encoding:") != std::string::npos)
			{
				d_header->transfer_encoding = (buffer.substr(pos + 1));
			}
			if (buffer.size() == 1 && buffer == "\r")
			{
				break ;
			}
		}
		if (header.find("\r\n\r\n") != std::string::npos)
			header.erase(0, header.find("\r\n\r\n") + 4);
		else
		{
			d_header->res_status = 431;
			return ;
		}
		/* check if the requested uri exist in locations block */
		if (find_required_location() < 0)
			return ;
		/* requested uri is exist */
		
		/* if this location have a redirection */
		else if (check_for_redirection() < 0)
			return ;
		/* if this method is allowed in this location */
		else if (allowed_methods() < 0)
			return ;
		if (d_header->transfer_encoding.empty() == false)
		{
			
			if (d_header->transfer_encoding.find("chunked") == std::string::npos 
			|| (d_header->boundary.empty() == false 
			&& d_header->transfer_encoding.find("chunked") == std::string::npos))
			{
				d_header->res_status = 501;
				return ;
			}
		}
	}
	if (header.empty() == false && (d_header->method == "GET" || d_header->method == "DELETE"))
		read_body(header);
	else if (d_header->method == "POST") 
	{
		if (dir_to_upload.empty() == true)
		{
			if (check_for_upload() == -1)
			{
				end_of_file = true;
				return ;
			}
		}
		if (d_header->Content_type.empty() == true)
		{
			d_header->res_status = 415;
			return ;
		}
		if (d_header->transfer_encoding.empty() == true && d_header->Content_Length ==  -2)
		{
			d_header->res_status = 400;
			return;
		}
		if (d_header->transfer_encoding.empty() == false)
		{
			save_chunk_improve(header);
		}
		else // needs to check for cgi  
		{
			save_binary(header);
		}
	}
	else if (d_header->method.find("GET") != std::string::npos || d_header->method.find("DELETE") != std::string::npos)
	{
		end_of_file = true;
		return ;
	}
}

int		request::check_for_upload() {
	std::map<std::string, std::string>::const_iterator iter;

	iter =  d_header->it->second.find("upload");
	if (iter != d_header->it->second.cend())
	{
		dir_to_upload = iter->second;
		//error still handling it 
		// if (access(dir_to_upload.c_str(), W_OK) != -1){
		// 	d_header->res_status = 403;
		// 	return -1;
		// }
		return 1;
	}
	return -1;
}


void	request::parse_the_uri()
{
	int			find;

	d_header->new_uri = d_header->uri;
	find = d_header->new_uri.find_last_of('?');
	if (std::string::npos != find)
	{
		d_header->query = d_header->uri;
		d_header->new_uri.resize(find);
		d_header->query.erase(0, find + 1);
	}
}

int	request::update_the_uri( std::string &uri )
{
	int			find;

	find = uri.find_last_of('/');
	if (find && std::string::npos != find)
	{
		uri.resize(find);
		return (1);
	}
	return (0);
}

int	request::find_required_location( )
{
	std::string	updated_uri;
	int result;
	
	parse_the_uri();
	d_header->it = server_data->location.find(d_header->new_uri);
	result = 1;
	updated_uri = d_header->new_uri;
	while (result)
	{
		if (d_header->it != server_data->location.cend())
		{
			// std::cout << "found the matching location for the request uri." << std::endl;
			std::cout << "your location is ... : " << d_header->it->first << std::endl;
			return (0);
		}
		result = update_the_uri(updated_uri);
		d_header->it = server_data->location.find(updated_uri + "/");
	}
	d_header->res_status = 404;
	return (-1);
}

int	request::check_for_redirection()
{
	std::map<std::string, std::string>::const_iterator iter;
	std::stringstream	mystream;
	std::string			result;
	
	iter = d_header->it->second.find("redirect");
	if (iter != d_header->it->second.cend())
	{
		mystream << iter->second;
		mystream >> result;
		d_header->is_redirect = true;
		d_header->res_status  = std::atoi(result.c_str());
		mystream >> result;
		d_header->redirect_path = result;
		return (-1);
	}
	return (0);
}


int 	request::allowed_methods()
{
	std::map<std::string, std::string>::const_iterator iter;
	std::stringstream	my_stream;
	std::string			method;

	iter = d_header->it->second.find("allowed_methods");
	if (iter != d_header->it->second.cend())
	{
		my_stream << iter->second;
		d_header->methods = iter->second;
		while ( my_stream >> method	)
		{
			if (method == d_header->method)
				return (0);
		}
	}
	d_header->res_status = 405;
	return (-1);
}
