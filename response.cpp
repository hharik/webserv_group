/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 12:25:24 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/04 01:25:44 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "message.hpp"
#include "parsing.hpp"
#include "response.hpp"

response::response( const data_serv *dptr,  data_header *hptr ):server_data(dptr), header_data(hptr)
{
	status_code = 0;
	is_header = true;
	// is_redirect = false;
	is_body = false;
	eof = false;
	is_read = false;
	/*---------------*/
}

void	response::response_handler( int client_fd, std::string& body )
{
	body.clear();
	if (is_header == true)
	{
		if (!header_data->res_status && header_data->method == "GET")
			Get_method();
		else if (!header_data->res_status && header_data->method == "POST")
			Post_method();
		else if (!header_data->res_status && header_data->method == "DELETE")
			Delete_method();
		create_header();
		send(client_fd, header.c_str(), header.size(), 0);
		is_header = false;
		is_body = true;
		std::cout << "----------------* HEADER *----------------" << std::endl;
		std::cout << header << std::endl;
		std::cout << "----------------* HEADER *----------------" << std::endl;
	}
	else if (is_body == true)
	{
		if (header_data->method == "GET")
			GET_handler(client_fd);
		else if (header_data->method == "POST")
			Post_method();
		else if (header_data->method == "DELETE")
			Delete_method();
	}
}

void	response::GET_handler( int client_fd )
{
	if (is_read == false)
		in_file.open(requested_resource.c_str());
	is_read = true;
	if (in_file.is_open())
	{
		memset(buffer, 0, BUFFER_SIZE);
		in_file.read(buffer, BUFFER_SIZE);
		std::streamsize size = in_file.gcount();
		if (send(client_fd, buffer, size, 0) < 0)
		{
			perror("send");
			exit(EXIT_FAILURE);
		}
		else if (in_file.eof())
			eof = true;
	}
	else
		eof = true;
}

int response::upload_file()
{
	if (header_data->transfer_encoding.empty() == false)
	{
		
		if (header_data->transfer_encoding.find("chunked") == std::string::npos 
		|| (header_data->boundary.empty() == false 
		&& header_data->transfer_encoding.find("chunked") == std::string::npos))
		{
			header_data->res_status = 501;
			return 0;
		}
	}
	if (header_data->method == "POST") 
	{
		if (header_data->Content_type.empty() == true)
		{
			header_data->res_status = 415;
			return  0;
		}
		if (header_data->transfer_encoding.empty() == true && header_data->Content_Length ==  -2)
		{
			header_data->res_status = 400;
			return 0;
		}
		// else if (header_data->transfer_encoding.empty() == false)
		// {
		// 		save_chunk_improve(header);
		// }
		// else // needs to check for cgi  
		// {
		// 		save_binary(header);
		// }
	}
	return 0;
}

int	response::is_file_or_directory( const char *path )
{
	struct	stat	spath;

	if (stat(path, &spath) == 0)
	{
		if (S_ISREG(spath.st_mode))
			return (0);
		else if (S_ISDIR(spath.st_mode))
			return (1);
	}
	return (-1);
}

const std::string	response::get_extension( const std::string& target )
{
	std::string	result(target);
	int	ind;

	ind = target.find_last_of('.');
	result.erase(0, ind);
	return (result);
}

int		response::serve_the_file()
{
	if (access(requested_resource.c_str(), F_OK) < 0)
		status_code = 404;
	else if (access(requested_resource.c_str(), R_OK) < 0)
			status_code = 403;
	else
	{
		/* if location support cgi */
		/* pass file to cgi*/
		std::cout << "your requested resource is : " << requested_resource << std::endl;
		if (search_inside_location("cgi " + get_extension(requested_resource)) == 1)
		{
			std::cout << "location support the cgi" << std::endl;
		}
		/* if loaction does not has a cgi */
		/* send the requested file with 200 status code */
		// else 
		// {
		// 	std::cout << "send the requested file with 200 status code " << std::endl;
		// }
		status_code = 200;
	}
	return (0);
}

int		response::requested_resource_is_dir()
{
	std::string	index;
	int			ind;

	ind = header_data->uri.size() - 1;
	/* requesting a directory without '/' at the end */
	if (header_data->uri[ind] != '/')
	{
		std::cout << "301 Moved Permanently" << std::endl;
		status_code = 301;
		return (0);
	}
	/* has index file */
	search_inside_location("index");
	if (iter != header_data->it->second.cend())
	{
		index = iter->second;
		requested_resource = target + index;
		serve_the_file();
	}
	else if (search_inside_location("auto_indexing") == 1)
	{
		if (iter->second == "off")
		/* if auto index off */
		{
			std::cout << "403 Forbidden " << std::endl;
			status_code = 403;
			return (0);
		}
		/* if auto index on */
		else if (iter->second == "on")
		{
			std::cout << "call auto index " << std::endl;
			// call auto_index();
		}
	}
	return (0);
}

int	response::search_inside_location( const std::string to_find )
{
	iter = header_data->it->second.find(to_find);
	if (iter != header_data->it->second.cend())
		return (1);
	return (0);
}

/* GET method */
void	response::Get_method()
{
	int			status;
	std::string rest;

	std::cout << "GET Method ... " << std::endl;

	/* get_requested_resource */
	search_inside_location("root");
	rest = header_data->uri;
	rest.erase(0, header_data->new_uri.size() + 1);
	target = iter->second + rest;

	/* check if the path exist */
	if (access(target.c_str(), F_OK) < 0)
	{
		status_code = 404;
		return ;
	}
	status = is_file_or_directory(target.c_str());
	if (status == 0)
	{
		std::cout << "target is a file ... " << std::endl;
		requested_resource = target;
		serve_the_file();
		// requested_resource_is_file();
	}
	else if (status == 1)
	{
		std::cout << "targer is a directory ... " << std::endl;
		requested_resource_is_dir();
	}
	else 
	{
		std::cout << "no_such_file_or_directory ... " << std::endl;
	}	
}


/* POST method */
void	response::Post_method()
{
	std::cout << "POST method ... " << std::endl;
}

/* Delete method */
void	response::Delete_method()
{
	std::cout << "DELETE method ... " << std::endl;	
}

std::string	response::get_start_line()
{
	/* HTTP/1.1 code phrase               */
	std::string	start = HTTP_V;
	start += std::to_string(header_data->res_status);
	if (header_data->res_status == 200)
		start += " OK\r\n";
	else if (header_data->res_status == 301)
		start += " Moved Permanently\r\n";
	else if (header_data->res_status == 302)
		start += " Found\r\n";
	else if (header_data->res_status == 403)
		start += " Forbidden\r\n";
	else if (header_data->res_status == 404)
		start += " Not Found\r\n";
	else if (header_data->res_status == 400)
		start += " Bad Request\r\n";
	else if (header_data->res_status == 414)
		start += " URI Too Long\r\n";
	else if (header_data->res_status == 431)
		start += " Request Header Fields Too Large\r\n";
	else if (header_data->res_status == 415)
		start += " Unsupported Media Type\r\n";
	else if (header_data->res_status == 501)
		start += " Not Implemented\r\n";
	return (start);
}

std::string	response::Get_Content_Type()
{
	/* Content-Type: text/html\r\n              */
	std::map<std::string, std::string>::iterator iterator;
	std::string	Content_Type = "Content-Type: ";
	std::string	result;
	int	find;

	result = requested_resource;
	find = result.find_last_of('.');
	if (find != std::string::npos)
	{
		result.erase(0, find + 1);
		iterator = parsing::mime_types_ay.find(result);
		if (iterator != parsing::mime_types_ay.end())
		{
			Content_Type += iterator->second;
			Content_Type += "\r\n";
			return (Content_Type);
		}
	}
	Content_Type += DEFAULT_MIME_TYPE;
	Content_Type += "\r\n";
	return (Content_Type);
}

std::string response::time_date()
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

std::string	response::Get_Content_Length()
{
/* Content-Length: 1234\r\n\r\n             */
	std::string Content_Length = "Content-Length: ";	
	struct stat s;
	
    if (stat(requested_resource.c_str(), &s) == 0)
	{
		Content_Length += std::to_string(s.st_size);
        Content_Length += "\r\n\r\n";
		return Content_Length;
    }
	return (Content_Length + "0 \r\n\r\n");
}

void	response::Error_pages()
{
	iter = header_data->it->second.find("error");
	std::cout << "error : pages " << iter->second << std::endl;

}

void response::create_header()
{
	/* HTTP/1.1 200 OK\r\n                      */
	/* Date: Wed, 11 May 2023 12:00:00 GMT\r\n  */
	/* Server: Apache/2.4.38 (Unix)\r\n         */
	/* Content-Type: text/html\r\n              */
	/* Content-Length: 1234\r\n\r\n             */

	header = get_start_line();
	header += " Date: ";
	header += time_date() + "\r\n";
	header += SERVER;
	
	if (header_data->res_status == 301)
	{
	/* Location: http://example.com/dir/dir2/dir3/  */
		header += "Location: ";
		if (header_data->redirect_path[0] != '/')
			header += header_data->redirect_path + "\r\n";
		else
		{
			std::cout << "target: " << target << std::endl;
			header += target + header_data->redirect_path;
			header += "\r\n";
		}
	}
	else if (header_data->res_status == 405)
	{
			header += "Allow: ";
			header += header_data->methods + "\r\n";
	}
	if (header_data->res_status != 200)
		Error_pages();
	header += Get_Content_Type();
	header += Get_Content_Length();
}