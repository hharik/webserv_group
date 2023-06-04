
#include "message.hpp"
#include "request.hpp"
#include <sstream>
#include <string>

request::request( const data_serv *dptr, data_header *hptr ) : server_data(dptr), d_header(hptr), rsize(0) , size(0), chunked_size(-2)
{
	end_of_header = false;
}

request::~request()
{
	
}

void request::parse(std::string &header) 
{
	std::string tes;
	if (d_header->Host.empty() == true)
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
	end_of_header = true;
	std::cout << "status : " << std::endl;
	return ;
	// if (d_header->transfer_encoding.empty() == false)
	// {
		
	// 	if (d_header->transfer_encoding.find("chunked") == std::string::npos 
	// 	|| (d_header->boundary.empty() == false 
	// 	&& d_header->transfer_encoding.find("chunked") == std::string::npos))
	// 	{
	// 		d_header->res_status = 501;
	// 		return ;
	// 	}
	// }
	
	// if (d_header->method == "POST") 
	// {
	// 	if (d_header->Content_type.empty() == true)
	// 	{
	// 		d_header->res_status = 415;
	// 		return ;
	// 	}
	// 	if (d_header->transfer_encoding.empty() == true && d_header->Content_Length ==  -2)
	// 	{
	// 		d_header->res_status = 400;
	// 		return;
	// 	}
	// 	else if (d_header->transfer_encoding.empty() == false)
	// 	{
	// 		save_chunk_improve(header);
	// 	}
	// 	else // needs to check for cgi  
	// 	{
	// 		save_binary(header);
	// 	}
	// }
	// else if (d_header->method.find("GET") != std::string::npos)
	// {
	// 	end_of_file = true;
	// 	return ;
	// }
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

int	request::update_the_uri()
{
	int			find;

	find = d_header->new_uri.find_last_of('/');
	if (find && std::string::npos != find)
	{
		d_header->new_uri.resize(find);
		return (1);
	}
	return (0);
}

int	request::find_required_location( )
{
	int result;
	int	found;
	
	parse_the_uri();
	d_header->it = server_data->location.find(d_header->new_uri);
	result = 1;
	while (result)
	{
		std::cout << "new_uri : " << d_header->new_uri << std::endl;
		if (d_header->it != server_data->location.cend())
		{
			std::cout << "found the matching location for the request uri." << std::endl;
			// std::cout << "your location is ... : " << it->first << std::endl;
			return (0);
		}
		result = update_the_uri();
		d_header->it = server_data->location.find(d_header->new_uri);
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
			{
				std::cout << "this location support the " << method << " method" << std::endl;
				return (0);
			}
		}
	}
	d_header->res_status = 405;
	return (-1);
}
