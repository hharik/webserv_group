/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 12:25:24 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/01 02:44:31 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "message.hpp"
#include "parsing.hpp"

response::response( const data_serv *dptr, const data_header *hptr ):server_data(dptr), header_data(hptr)
{
	is_file = true;
}

void	response::parse_the_uri()
{
	int			find;
	int			size;

	find = header_data->uri.find_last_of('?');
	if (std::string::npos != find)
	{
		std::cout << "change the uri ? " << std::endl;
		new_uri.resize(find);
		size = header_data->uri.size() - new_uri.size() - 1;
		query.resize(size);
		std::copy_n(header_data->uri.begin(), find, new_uri.begin());
		std::copy_n(header_data->uri.begin() + find + 1, size, query.begin());
	}
	else
		new_uri = header_data->uri;
	std::cout << "uri : " << new_uri << std::endl;
}

int	response::update_the_uri()
{
	int			find;
	int			size;

	is_file = true;
	find = new_uri.find_last_of('/');
	if (std::string::npos != find)
	{
		if (new_uri[find] == '/')
			is_file = false;
		if (find)
		{
			new_uri.resize(find);
			return (1);
		}
	}
	return (0);
}

int	response::find_required_location( )
{
	std::map<std::string, std::map<std::string, std::string> >::const_iterator it;
	int result;
	int	found;

	result = 1;
	parse_the_uri();
	while (result)
	{
		if (is_file == false)
			it = server_data->location.find(new_uri + "/");
		else
			it = server_data->location.find(new_uri);
		if (it != server_data->location.cend())
		{
			std::cout << "found the matching location for the request uri." << std::endl;
			std::cout << "your location is ... : " << it->first << std::endl;
			return (1);
		}
		result = update_the_uri();
	}
	return (0);
}

// void	response::set_start_line( const std::string http_version, int status )
// {
// 	std::string ver = "http1.1";
// 	std::string str = " 200 OK";
// 	start_line = ver;
// 	start_line.append(str);
// 	std::cout << start_line << std::endl;

// 	start_line = http_version;
// 	start_line.append(str);
// 	std::cout << start_line << std::endl;	
// }

// void response::create_header()
// {
// 	msg = "HTTP/1.1 " + std::to_string(client_request.getData_header().res_status);
// 	if (client_request.getData_header().res_status == 501){
// 		msg += " Not implemented\r\nContent-Type: text/html\r\n\r\n";
// 		msg += "<html><body><h1>501 - Not implemented!</h1></body></html>";
// 	}
// 	else if (client_request.getData_header().res_status == 400){
// 		msg + " Bad Request\r\nContent-Type: text/html\r\n\r\n";
// 		msg + "<html><body><h1>400 - Bad request!</h1></body></html>";

// 	}
// 	else if (client_request.getData_header().res_status == 414){
// 		msg + " Request-URI Too Long\r\nContent-Type: text/html\r\n\r\n";
// 		msg + "<html><body><h1>414 - Request-Uri Too large!</h1></body></html>";
	
// 	}
// 	else if (client_request.getData_header().res_status == 413){
// 		msg += " Request Entity Too large\r\nContent-Type: text/html\r\n\r\n";
// 		msg += "<html><body><h1>413 - Request Entity Too large!</h1></body></html>";

// 	}
// 	else if (client_request.getData_header().res_status == 404){
// 		msg += " Not Found\r\nContent-Type: text/html\r\n\r\n";
// 		msg += "<html><body><h1>404 - Not Found!</h1></body></html>";
// 	}
// 	else if (client_request.getData_header().res_status == 301){
// 		msg += " Moved Permanently\r\nContent-Type: text/html\r\n\r\n";
// 		msg += "<html><body><h1>301 Moved Permanently!</h1></body></html>";
// 	}
// 	else if (client_request.getData_header().res_status == 405){

// 		msg += " Method Not Allowed\r\nContent-Type: text/html\r\n\r\n";
// 		msg += "<html><body><h1>405 - Method Not Allowed!</h1></body></html>";
// 	}
// 	else if (client_request.getData_header().res_status == 403){
// 		msg += " Forbidden\r\nContent-Type: text/html\r\n\r\n";
// 		msg += "<html><body><h1>403 - Forbidden!</h1></body></html>";
// 	}
// 	else if (client_request.getData_header().res_status == 200){
// 		msg += " OK\r\nContent-Type: text/html\r\n\r\n";
// 	}
// 	else if (client_request.getData_header().res_status == 201)
// 	{
// 		msg += " Created\r\nContent-Type: text/html\r\n\r\n";
// 	}
// 	else if (client_request.getData_header().res_status == 431)
// 		msg += " Request Header Fields Too Large\r\nContent-Type: text/plain\r\n\r\n";
// 		msg += "The server cannot process the request because the provided headers are too large. Please reduce the size of the headers and try again.\r\n";
// }