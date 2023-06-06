/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 12:25:24 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/06 10:29:35 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "client.hpp"
#include "response.hpp"
#include <string>

response::response( const data_serv *dptr,  data_header *hptr ):server_data(dptr), header_data(hptr)
{
	eof = false;
	is_open = false;
	default_response = false;
	/*---------------*/
}

void	response::response_handler( int client_fd )
{
	if (header_data->res_status == 200 || header_data->res_status == 201)
	{
		if (header_data->method == "GET")
			Get_method();
		else if (header_data->method == "POST")
			Post_method();
		else if (header_data->method == "DELETE")
			Delete_method();
	}
	if (is_open == false)
	{
		create_header();
		Send_the_Header(client_fd);
		std::cout << "----------------* HEADER *----------------" << std::endl;
		std::cout << header << std::endl;
		std::cout << "----------------* HEADER *----------------" << std::endl;
	}
	else
		Send_the_Body(client_fd);
}

void	response::Send_the_Header( int client_fd )
{
	in_file.open(requested_resource.c_str());
	is_open = true;
	if (send(client_fd, header.c_str(), header.size(), 0) < 0)
	{
		perror("send");
		exit(EXIT_FAILURE);
	}
}

void	response::Send_the_Body( int client_fd )
{
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
	{
		// Default = Default_Response( std::to_string(header_data->res_status), response_content);
		std::cout << "response size  : "  << response_content.size() << std::endl;
		if (send(client_fd, response_content.c_str(), response_content.size(), 0) < 0)
		{
			perror("send");
			exit(EXIT_FAILURE);
		}
		eof = true;
	}
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
		header_data->res_status = 404;
	else if (access(requested_resource.c_str(), R_OK) < 0)
			header_data->res_status = 403;
	else
	{
		/* if location support cgi */
		/* pass file to cgi*/
		std::cout << "your requested resource is : " << requested_resource << std::endl;
		
		if (search_inside_location("cgi " + get_extension(requested_resource)) == 1)
		{
			std::cout << "query : " << header_data->query << std::endl;
			std::cout << "location support the cgi" << std::endl;
		}
		/* if loaction does not has a cgi */
		/* send the requested file with 200 status code */
		// else 
		// {
		// 	std::cout << "send the requested file with 200 status code " << std::endl;
		// }
		header_data->res_status = 200;
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
		header_data->res_status = 301;
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
			header_data->res_status = 403;
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

void	response::get_requested_resource()
{
	std::string rest;

	// std::cout << "GET Method ... " << std::endl;

	/* get_requested_resource */
	search_inside_location("root");
	rest = header_data->uri;
	rest.erase(0, header_data->new_uri.size() + 1);
	target = iter->second + rest;
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
	std::cout << "iter :  " << iter->second << std::endl;
	rest = header_data->uri;
	rest.erase(0, header_data->new_uri.size() + 1);
	target = iter->second + rest;
	/* check if the path exist */
	if (access(target.c_str(), F_OK) < 0)
	{
		header_data->res_status = 404;
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

int	response::delete_the_file()
{
	/* remove the file if the return 0 that mean it successed .. */
	
	/* first let's check if this file has the permission */
	if (access(requested_resource.c_str(), W_OK) < 0) 
	{
		header_data->res_status = 403;
		return (0);
	}
	/* remove this file */
	if (std::remove("file ") == 0)
	{
		header_data->res_status = 204;
		return(0);
	}
	std::cout << "can't remove this file ... premission denied!" << std::endl;
	header_data->res_status = 403;
	return (0);
}

int	response::delete_dir( const char *directory )
{
    struct dirent	*dp;
	DIR 			*dir;
	std::string 	str;
	int				status;

	if ((dir = opendir (directory)) == NULL)
	{
		header_data->res_status = 403;
		return -1;
    }
	status = -2;
    while ((dp = readdir (dir)) != NULL)
	{
		str = directory;
		/* check if this path is file or directory */
		if (CMP(dp->d_name))
		{
			str += "/";
			str += dp->d_name;
			status = is_file_or_directory(str.c_str());
		}
		/* first remove all files */
		if (status == 0 && CMP(dp->d_name))
		{
			if (remove(str.c_str()) != 0)
			{
				header_data->res_status = 403;
				return -1;
			}
		}
		/* than remove all subdirectories recessively */
		else if (status == 1 && CMP(dp->d_name)) 
		{
			delete_dir(str.c_str());
			if (remove(str.c_str()) != 0)
			{
				header_data->res_status = 403;
				return -1;
			}
		}
	}
	header_data->res_status = 204;
	closedir(dir);
	return 0;
}

/* Delete method */
void	response::Delete_method()
{
	int			status;
	std::cout << "DELETE method ... " << std::endl;

	get_requested_resource();
	/* check if the path exist */
	if (access(target.c_str(), F_OK) < 0)
	{
		header_data->res_status = 404;
		return ;
	}
	status = is_file_or_directory(target.c_str());
	if (status == 0)
	{
		std::cout << "target is a file ... " << std::endl;
		requested_resource = target;
		delete_the_file();
		// requested_resource_is_file();
	}
	else if (status == 1)
	{
		std::cout << "targer is a directory ... " << std::endl;
		if (header_data->uri[header_data->uri.size() - 1] != '/')
		{
			std::cout << "409 Conflict" << std::endl;
			header_data->res_status = 409;
			return ;
		}
		target.resize(target.size() - 1);
		delete_dir(target.c_str());
	}
	else 
	{
		status = 404;
		std::cout << "no_such_file_or_directory ... " << std::endl;
	}
	
}

std::string	response::Default_Response(const std::string &status_code, const std::string &Meaning)
{
	std::string	html_resp = "<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<title>";
	html_resp += status_code;
	html_resp += Meaning;
	std::string	rest = "</title>\r\n</head>\r\n</html>\r\n";
	return (html_resp);
}

std::string	response::get_start_line()
{
	/* HTTP/1.1 code phrase               */
	std::string	start = HTTP_V;
	start += std::to_string(header_data->res_status);
	if (header_data->res_status == 200)
		response_content += S200;
	else if (header_data->res_status == 201)
		response_content += S201;
	else if (header_data->res_status == 204)
		response_content += S204;
	else if (header_data->res_status == 301)
		response_content += S301;
	else if (header_data->res_status == 302)
		response_content += S302;
	else if (header_data->res_status == 400)
		response_content += S400;
	else if (header_data->res_status == 403)
		response_content += S403;
	else if (header_data->res_status == 404)
		response_content += S404;
	else if (header_data->res_status == 405)
		response_content += S405;
	else if (header_data->res_status == 409)
		response_content += S409;
	else if (header_data->res_status == 414)
		response_content += S414;
	else if (header_data->res_status == 415)
		response_content += S415;
	else if (header_data->res_status == 431)
		response_content += S431;
	else if (header_data->res_status == 501)
		response_content += S501;
	start += response_content;
	start += "\r\n";
	return (start);
}


std::string	response::Get_Content_Type()
{
	/* Content-Type: text/html\r\n              */
	std::map<std::string, std::string>::iterator iterator;
	std::string	Content_Type = "Content-Type: ";
	std::string	result;
	int	find;
	
	if (default_response == false)
	{
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
	}
	else
		Content_Type += "text/html";
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
	std::string	Default_Length;
	struct stat s;
	
	if (default_response == false)
	{
    	if (stat(requested_resource.c_str(), &s) == 0)
		{
			Content_Length += std::to_string(s.st_size);
    	    Content_Length += "\r\n\r\n";
			return Content_Length;
    	}
	}
	Default_Length = std::to_string(header_data->res_status);
	return (Content_Length + Default_Length + "\r\n\r\n");
}

void	response::Pages()
{
	if (header_data->res_status >= 400)
	{
		er_it = server_data->errors.find(header_data->res_status);
		requested_resource = er_it->second;
		if (access(er_it->second.c_str(), F_OK) == 0)
			return ;
	}
	if (header_data->method != "GET")
	{
		response_content = Default_Response( std::to_string(header_data->res_status) , response_content );
		default_response = true;
	}
}

void response::create_header()
{
	/* HTTP/1.1 200 OK\r\n                      */
	/* Date: Wed, 11 May 2023 12:00:00 GMT\r\n  */
	/* Server: Apache/2.4.38 (Unix)\r\n         */
	/* Content-Type: text/html\r\n              */
	/* Content-Length: 1234\r\n\r\n             */

	header = get_start_line();
	header += "Date: ";
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
			header += target + header_data->redirect_path;
			header += "\r\n";
		}
	}
	else if (header_data->res_status == 405)
	{
			header += "Allow: ";
			header += header_data->methods + "\r\n";
	}
	Pages();
	header += Get_Content_Type();
	header += Get_Content_Length();
}

bool	response::Is_End_Of_File()
{
	return(eof);	
}