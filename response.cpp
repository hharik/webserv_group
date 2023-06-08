/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 12:25:24 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/08 19:53:47 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "client.hpp"
#include "response.hpp"
#include <string>
#include <sys/stat.h>

response::response( const data_serv *dptr,  data_header *hptr ):server_data(dptr), header_data(hptr)
{
	eof = false;
	is_open = false;
	default_response = false;
	s204 = 0;
	s403 = 0;
	is_alive = false;
	auto_index = false;
	/*---------------*/
}

response::~response( )
{
	if (is_alive == true)
		kill(pid, 0);
	std::cout << " Destruct Response : " << std::endl;
}

void	response::response_handler( int client_fd )
{
	if (is_open == false && header_data->res_status < 400 && header_data->is_redirect == false)
	{
		// std::cout << "status : " << header_data->res_status << std::endl;
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
		Send_the_Body( client_fd );
}

void	response::Send_the_Header( int client_fd )
{
	in_file.open(header_data->requested_resource.c_str());
	is_open = true;
	if (send(client_fd, header.c_str(), header.size(), 0) < 0)
	{
		perror("send_header : ");
		eof = true;
		return ;
	}
}

void	response::Send_the_Body( int client_fd )
{
	if (in_file.is_open() == true && default_response == false && auto_index == false)
	{
		memset(buffer, 0, BUFFER_SIZE);
		in_file.read(buffer, BUFFER_SIZE);
		std::streamsize size = in_file.gcount();
		if (send(client_fd, buffer, size, 0) < 0)
		{
			perror("send_body : ");
			eof = true;
			return ;
		}
		else if (in_file.eof())
			eof = true;
	}
	else if (default_response == true)
	{
		std::cout << response_content << std::endl;
		if (send(client_fd, response_content.c_str(), response_content.size(), 0) < 0)
		{
			perror("send");
		}
		eof = true;
	}
	else if (auto_index == true)
	{
		if (send(client_fd, auto_index_content.c_str(), auto_index_content.size(), 0) < 0)
		{
			perror("send");
		}
		eof = true;
	}
}



const std::string	response::get_extension( const std::string& target )
{
	std::string	result(target);
	int	ind;

	ind = target.find_last_of('.');
	result.erase(0, ind);
	return (result);
}

int	response::file_status()
{
	int	ind;

	ind = target.size() - 1;
	if (access(target.c_str(), F_OK))
	{
		if (target[ind] == '/')
			target.resize(ind);
		if (access(target.c_str(), F_OK))
		{
			header_data->res_status = 404;
			return (-1);
		}
	}
 	if (access(target.c_str(), R_OK))
	{
		header_data->res_status = 403;
		return (-1);
	}
	return (0);
}

std::string time_date()
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

void	response::handle_cgi(std::string &request_file)
{



	/* for POST REQUEST'S WE READ FROM THE FILE THAT WAS UPLOADED TO US 
	AND GIVE IT AS AN INPUT TO THE EXECVE */

	search_inside_location("cgi " + get_extension(request_file));
	/*file to store*/
	std::cout << "HELLO " << std::endl;
	file_tmp = "/tmp/" + time_date();
	cgifd[1] = open(file_tmp.c_str(), O_CREAT | O_RDWR, 0644);

	/* env to store	*/
	std::string tem;
	tem = "SCRIPT_FILENAME=" +  request_file;
	Env.push_back(tem);
	Env.push_back("REQUEST_METHOD=" + header_data->method);
	Env.push_back("REDIRECT_STATUS=200");
	if (header_data->query.empty() == false)
	{
		Env.push_back("QUERY_STRING=" + header_data->query);
		std::cout <<  "*  *" <<  header_data->query << std::endl;
	}

	if (header_data->Content_Length != -2)
 	{
		std::stringstream len;
		len << header_data->Content_Length;
		std::string content_len;
		len >> content_len;
		Env.push_back("CONTENT_LENGTH=" + content_len);
	}

	char **env;
	/* data to execute*/
	std::cout << "HEKEI 0" << std::endl;
	agv[0] = (char *)iter->second.c_str();  //executable
	agv[1] = (char *)request_file.c_str(); //requested script
	agv[2] = NULL;
	std::cout << agv[0] << std::endl;
	std::cout << agv[1] << std::endl;

	env = new char *[Env.size()];
	for (size_t i = 0; i < Env.size(); i++)
	{
		env[i] = (char *) Env[i].c_str();
		std::cout <<" (" << env[i] << ")" << std::endl;
	}
	// env = NULL;
	env[Env.size()] = NULL;
	pid = fork();
	if(pid == -1)
		perror("fork");
	else if (pid == 0)
	{
		//child process 
		dup2(cgifd[1], 1);
		close(cgifd[1]);
		if (header_data->method == "POST")
		{
			cgifd[0] = open(request_file.c_str(), O_RDONLY);
			dup2(cgifd[0], 0);
			close(cgifd[0]);
		}
		execve(agv[0], agv, env);
		perror("execve");
		exit(EXIT_SUCCESS);
	}
	std::ifstream file(file_tmp);
	std::stringstream str;
	std::string buffer;
	str << file.rdbuf();
	buffer = str.str();
	std::cout << " ////////////////////////////" << std::endl;
	std::cout<<buffer<<std::endl;
	std::cout << " ////////////////////////////" << std::endl;
}


int		response::serve_the_file()
{
	{
		// std::cout << "Target : " << target << std::endl;
		target = header_data->requested_resource;
		if (file_status() == 0)
		{
		/* if location support cgi */
		/* pass file to cgi*/
		// std::cout << "your requested resource is : " << header_data->requested_resource << std::endl;
		if (search_inside_location("cgi " + get_extension(header_data->requested_resource)) == 1)
		{
			std::cout << "query : " << header_data->query << std::endl;
			std::cout << "location support the cgi" << std::endl;
			std::cout << "requested_tr   ; " << header_data->requested_resource << std::endl;
			is_alive = true;
			handle_cgi(header_data->requested_resource);
			is_alive = false;
			std::cout << "file_tmp   :" << file_tmp << std::endl;
			header_data->requested_resource = file_tmp;
			header_data->res_status = 200;
		}
		else
			header_data->res_status = 200;
		// std::cout << "Serve this file any way ... " << std::endl;
		// std::cout << "file : " << header_data->requested_resource << std::endl;
		/* if loaction does not has a cgi */
		/* send the requested file with 200 status code */
		// else 
		// {
			// std::cout << 
		// 	std::cout << "send the requested file with 200 status code " << std::endl;
		// }
		/*	return depend on the cgi */
		}
		else
		{
			std::cout << "wn  :::  " << std::endl;
			std::cout << "file : " << header_data->requested_resource << std::endl;
			header_data->res_status = 404;
			// header_data->res_status = 404;
		}
	}
	return (0);
}

std::string response::generateAutoIndex(std::string &directory)
{
	std::string autoIndexHtml = "<html><body><ul>";
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(directory.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			std::string filename = ent->d_name;
			if (filename == ".")
				continue;
			// if (is_file_or_directory(std::string(directory + filename)) == 1)
			// 	filename.append("/");
			autoIndexHtml += "<li> <a href=\"" + filename + "\">" + filename + "</a></li>";
		}
		closedir(dir);
	}
	autoIndexHtml += "</url></body></html>";
	return autoIndexHtml;
}

int		response::requested_resource_is_dir()
{
	std::string	index;
	int			ind;

	ind = header_data->new_uri.size() - 1;
	/* requesting a directory without '/' at the end */
	if (header_data->new_uri[ind] != '/')
	{
		header_data->res_status = 301;
		header_data->new_uri += "/";
		return (0);
	}
	/* has index file */
	search_inside_location("index");
	if (iter != header_data->it->second.cend())
	{
		std::cout << "has an index file " << std::endl;
		index = iter->second;
		header_data->requested_resource = target + index;
		serve_the_file();
	}
	else if (search_inside_location("auto_indexing") == 1)
	{
		if (iter->second == "off")
		/* if auto index off */
		{
			header_data->res_status = 403;
			return (0);
		}
		/* if auto index on */
		else if (iter->second == "on")
		{
			std::cout << "has an auto indexing " << std::endl;
			header_data->requested_resource = target;
			auto_index_content = generateAutoIndex(header_data->requested_resource);
			// std::cout << "auto index : " << auto_index_content.size() << std::endl;
			auto_index = true;
			header_data->res_status = 200;
			return (0);
		}
	}
	header_data->res_status = 404;
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

	/* get_header_data->requested_resource */
	// treat_target_resource("root");
	/* check if the path exist */
	// if (file_status())
	// 	return ;
	std::cout << "Requested_resource : " <<  header_data->requested_resource << std::endl;
	target = header_data->requested_resource;
	status = parsing::is_file_or_directory(header_data->requested_resource.c_str());
	if (status == 0)
	{
		header_data->requested_resource = target;
		serve_the_file();
	}
	else if (status == 1)
		requested_resource_is_dir();
	else
		std::cout << "no_such_file_or_directory ... " << std::endl;
}

void	upload_in_dir()
{
	
}


/* POST method */
void	response::Post_method()
{
	int			status;
	std::cout << "POST method ... " << std::endl;
	if (header_data->res_status == 301)
	{
		// std::cout << 
		header_data->new_uri += "/";
		return ;
	}
	handle_cgi(header_data->requested_resource);
	std::cout << "Requested_resource : " << header_data->requested_resource << std::endl;
	/* ---------------- */
	

	// treat_target_resource("upload");
	// // std::cout << "Target   : " << target << std::endl;
	// /* check if the path exist */
	// if (file_status())
	// 	return ;
	// status = parsing::is_file_or_directory(target.c_str());
	// if (status == 0)
	// {
	// 	std::cout << "target is a file ... " << std::endl;
	// 	header_data->requested_resource = target;
	// 	// requested_resource_is_file();
	// }
	// else if (status == 1)
	// {
	// 	upload_in_dir();
	// 	std::cout << "targer is a directory ... " << std::endl;
	// 	// requested_resource_is_dir();
	// }
	// else 
	// {
	// 	std::cout << "no_such_file_or_directory ... " << std::endl;
	// 	header_data->res_status = 404;
	// }	
	// search_inside_location("cgi");
}

int	response::delete_the_file()
{
	/* remove the file if the return 0 that mean it successed .. */
	
	/* first let's check if this file has the permission */
	if (access(header_data->requested_resource.c_str(), W_OK) < 0) 
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
		return -1;
	status = -2;
    while ((dp = readdir (dir)) != NULL)
	{
		str = directory;
		/* check if this path is file or directory */
		if (CMP(dp->d_name))
		{
			str += "/";
			str += dp->d_name;
			status = parsing::is_file_or_directory(str.c_str());
		}
		/* first remove all files */
		if (status == 0 && CMP(dp->d_name))
		{
			if (access(str.c_str(), W_OK) == 0)
			{
				if (remove(str.c_str()) == 0)
					s204++;
			}
			else
				s403++;
		}
		/* than remove all subdirectories recessively */
		else if (status == 1 && CMP(dp->d_name)) 
		{
			delete_dir(str.c_str());
			if (access(str.c_str(), W_OK) == 0)
			{
				if (remove(str.c_str()) == 0)
					s204++;
			}
			else
				s403++;
		}
	}
	closedir(dir);
	return 0;
}

void	response::treat_target_resource( const char *path )
{
	std::string	rest(header_data->new_uri);
	size_t		size;

	search_inside_location(path);
	size = header_data->it->first.size();
	if (header_data->it->first[size - 1] != '/')
		size++;
	rest.erase(0, size);
	target = iter->second + rest;
}

/* Delete method */
void	response:: Delete_method()
{
	int			status;

	std::cout << "DELETE method ... " << std::endl;
	// treat_target_resource("root");
	std::cout << "Target   : " << target << std::endl;
	/* check if the path exist */
	// if (access(target.c_str(), F_OK) < 0)
	// {
	// 	header_data->res_status = 404;
	// 	return ;
	// }
	status = parsing::is_file_or_directory(target.c_str());
	if (status == 0)
	{
		std::cout << "target is a file ... " << std::endl;
		header_data->requested_resource = target;
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
		std::cout << "Delete starting with this dir : " << target << std::endl;
		target.resize(target.size() - 1);
		delete_dir(target.c_str());

		if (s204 && !s403)
			header_data->res_status = 204;
		else if (s204 >= s403)
			header_data->res_status = 207;
		else if (s403 && !s204)
			header_data->res_status = 403;
		else if (s403 > s204)
			header_data->res_status = 500;
	}
	else 
		header_data->res_status = 404;
}

std::string	response::Default_Response(const std::string &status_code, const std::string &Meaning)
{
	std::string	html_resp = "<html>\r\n<body>\r\n<h1>";
	std::string	rest = "</h1>\r\n</body>\r\n</html>\r\n";
	html_resp += status_code;
	html_resp += Meaning;
	html_resp += rest;
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
	else if (header_data->res_status == 207)
		response_content += S207;
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
	else if (header_data->res_status == 500)
		response_content += S500;
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
	if (default_response == false && auto_index == false)
	{
		result = header_data->requested_resource;
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
		Content_Type += HTML;
	Content_Type += "\r\n";
	return (Content_Type);
}

std::string response::Get_Date()
{
	std::string	Date = "Date: ";

	/* Date: Wed, 11 May 2023 12:00:00 GMT\r\n  */
	time_t curr_time;
	tm * curr_tm;
	char date_string[100];
	char time_string[100];

	time(&curr_time);
	curr_tm = localtime(&curr_time);

	strftime(date_string, 50, "%B-%d-%Y-%T", curr_tm);
	Date += date_string;
	Date += "\r\n";
	return (Date);
}

std::string	response::Get_Content_Length()
{
/* Content-Length: 1234\r\n\r\n             */
	std::string Content_Length = "Content-Length: ";
	std::string	size;
	struct stat s;
	
	if (default_response == false && auto_index == false)
	{
    	if (stat(header_data->requested_resource.c_str(), &s) == 0)
		{
			Content_Length += std::to_string(s.st_size);
    	    Content_Length += "\r\n\r\n";
			return Content_Length;
    	}
	}
	if (default_response == true)
	{
		std::cout << "default content .... " << std::endl;
		size = std::to_string(response_content.size());
	}
	else if (auto_index == true)
		size = std::to_string(auto_index_content.size());
	return (Content_Length + size + "\r\n\r\n");
}

void	response::Pages()
{
	if (header_data->method == "GET" && header_data->res_status == 200)
	{
		std::cout << "not Default_conetent : ....." << std::endl;
		return ;
	}
	if (header_data->res_status >= 400)
	{
		er_it = server_data->errors.find(header_data->res_status);
		header_data->requested_resource = er_it->second;
		if (access(er_it->second.c_str(), F_OK) == 0)
			return ;
	}
	response_content = Default_Response( std::to_string(header_data->res_status), response_content);
	default_response = true;
}

std::string	response::get_Location()
{
	/* Location: http://example.com/dir/dir2/dir3/  */
	std::string		Location = "Location: ";
	
	if (header_data->is_redirect == true)
		Location += header_data->redirect_path;
	else
	{
		Location += header_data->new_uri;
		if (header_data->query.empty() == false)
		{
			Location += "?";
			Location += header_data->query;
		}
	}
	
	Location += "\r\n";
	return (Location);
}

void response::create_header()
{
	/* HTTP/1.1 200 OK\r\n                      */
	/* Date: Wed, 11 May 2023 12:00:00 GMT\r\n  */
	/* Server: Apache/2.4.38 (Unix)\r\n         */
	/* Content-Type: text/html\r\n              */
	/* Content-Length: 1234\r\n\r\n             */

	header = get_start_line();
	header += Get_Date();
	header += SERVER;
	
	if (header_data->res_status == 301 || header_data->is_redirect == true)
		header += get_Location();
	/* Location: http://example.com/dir/dir2/dir3/  */
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

void	response::set_eof( bool end_f )
{
	eof = end_f;	
}