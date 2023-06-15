/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 12:25:24 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/15 14:42:16 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "client.hpp"
#include "parsing.hpp"
#include "response.hpp"

response::response( const data_serv *dptr,  data_header *hptr ):server_data(dptr), header_data(hptr)
{
	eof = false;
	is_open = false;
	default_response = false;
	s204 = 0;
	s403 = 0;
	is_alive = false;
	auto_index = false;
	content_length = 0;
	sended_bytes = 0;
	/*---------------*/
}

response::~response( )
{
	if (is_alive == true)
		kill(pid, 0);
}

void	response::response_handler( int client_fd )
{

	if (is_open == false)
	{
		std::cout << "=============================" << std::endl;
		std::cout << "NEW URI                     : [" << header_data->new_uri << "]" << std::endl;
		std::cout << "METHOD                  : [" << header_data->method << "]" << std::endl;
		std::cout << "STATUS CODE             : [" << header_data->res_status << "]" <<  std::endl;
		std::cout << "Requested Resource      : [" << header_data->requested_resource << "]" << std::endl;
		std::cout << "cgi_script              : [" << header_data->cgi_script << "]" << std::endl;
		std::cout << "cgi_path                : [" << header_data->cgi_path << "]" << std::endl;
		if (header_data->_is_cgi == true)
			std::cout << "CGI                 : ON" << std::endl;
		else
		 	std::cout << "CGI                 : OFF" << std::endl;
		std::cout << "=============================" << std::endl;
	}	
	if (is_open == false && header_data->res_status < 400 && header_data->is_redirect == false)
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
		Send_the_Body( client_fd );
}

void	response::Send_the_Header( int client_fd )
{
	if (header_data->_is_cgi == true)
	{
		requested_file.open(cgi_output);
		requested_file.seekg(cgi_body_pos);
		is_open = true;
	}
	else
	{
		requested_file.open(header_data->requested_resource.c_str());
		is_open = true;
	}
	if (send(client_fd, header.c_str(), header.size(), 0) < 0)
	{
		perror("send_header : ");
		eof = true;
		return ;
	}
}

void	response::Send_the_Body( int client_fd)
{
	if (requested_file.is_open() == true && default_response == false)
	{
		memset(buffer, 0, BUFFER_SIZE);
		requested_file.read(buffer, BUFFER_SIZE);
		std::streamsize size = requested_file.gcount();
		if (send(client_fd, buffer, size, 0) < 0)
		{
			perror("send_body : ");
			eof = true;
			return ;
		}
		sended_bytes += size;
		if (sended_bytes >= content_length)
			eof = true;
	}
	else if (default_response == true)
	{
		if (send(client_fd, response_content.c_str(), response_content.size(), 0) < 0)
		{
			perror("send");
		}
		eof = true;
	}
}


int	response::file_status()
{
	int	ind;

	ind = header_data->requested_resource.size() - 1;
	if (access(header_data->requested_resource.c_str(), F_OK))
	{
		if (header_data->requested_resource[ind] == '/')
			header_data->requested_resource.resize(ind);
		if (access(header_data->requested_resource.c_str(), F_OK))
		{
			header_data->res_status = 404;
			return (-1);
		}
	}
 	if (access(header_data->requested_resource.c_str(), R_OK))
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

void	response::handle_cgi()
{


	/* cgi path ----> header_data->cgi_path .....*/
	/* for POST REQUEST'S WE READ FROM THE FILE THAT WAS UPLOADED TO US 
	AND GIVE IT AS AN INPUT TO THE EXECVE */

	// search_inside_location("cgi " + get_extension(request_file));
	/*file to store*/
	cgi_output = "/tmp/" + time_date();
	cgifd[1] = open(cgi_output.c_str(), O_CREAT | O_RDWR, 0644);

	/* env to store	*/
	Env.push_back("REQUEST_METHOD=" + header_data->method);
	Env.push_back("REDIRECT_STATUS=200");
	if (header_data->query.empty() == false)
	{
		Env.push_back("QUERY_STRING=" + header_data->query);
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
	agv[0] = (char*)header_data->cgi_path.c_str();
	if (header_data->method == "POST")
		agv[1] =  (char*)header_data->cgi_script.c_str();
	else
		agv[1] =  (char*)header_data->requested_resource.c_str();
	std::string tem;
	tem = "SCRIPT_FILENAME=" +  std::string(agv[1]);
	Env.push_back(tem);
	// agv[0] = (char *)header_data->cgi_path.c_str();  //executable
	// if (header_data->method == "POST")
	
	// 	agv[1] = (char *)header_data->cgi_script.c_str(); //requested script
	// else
	// 	agv[1] = (char *)request_file.c_str();
	std::cout << "AV[0] : " << agv[0] << std::endl;
	std::cout << "AV[1] : " << agv[1] << std::endl;
	std::cout << "REQUEST[0] : " << header_data->requested_resource << std::endl;

	Env.push_back("PATH_INFO=" + std::string(agv[1]));
	agv[2] = NULL;

	env = new char *[Env.size() * sizeof(char*)];
	for (size_t i = 0; i < Env.size(); i++)
	{
		env[i] = (char *) Env[i].c_str();
	}
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
			cgifd[0] = open(header_data->requested_resource.c_str(), O_RDONLY);
			dup2(cgifd[0], 0);
			close(cgifd[0]);
		}
		execve(agv[0], agv, env);
		perror("execve");
		exit(EXIT_SUCCESS);
	}
	close(cgifd[1]);
	wait(NULL);
	/* ===================== */

}


int		response::serve_the_file()
{
	if (file_status() == 0)
	{
		/* if location support cgi */
		/* pass file to cgi*/
		if (header_data->_is_cgi == true)
		{
			is_alive = true;
			handle_cgi();
			is_alive = false;
			// header_data->requested_resource = file_tmp;
			header_data->res_status = 200;
		/*	return depend on the cgi */
		}
		/* send the requested file with 200 status code */
		else
			header_data->res_status = 200;
	}
	return (0);
}

void response::generateAutoIndex(std::string &directory)
{
	std::string filena = "/tmp/AutoIndex_" + time_date() + ".html";
	std::fstream file;
	file.open(filena, std::ofstream::out );

	std::string autoIndexHtml = "<html><body> <title> Auto Index!! </title> <h1> Index of ";
	autoIndexHtml += header_data->new_uri + " </h1>  <hr> ";
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(directory.c_str())) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			std::string filename = ent->d_name;
			if (filename == ".")
				continue;
			if (Client::is_file_or_directory(std::string(directory + filename).c_str(), header_data) == 1)
				filename.append("/");
			autoIndexHtml += "<li> <a href=\"" + filename + "\">" + filename + "</a> </li>";
		}
		closedir(dir);
	}
	autoIndexHtml += "<hr> </url> </body> </html>";
	file << autoIndexHtml;
	file.close();
	header_data->requested_resource = filena;
}

int		response::requested_resource_is_dir()
{
	/* has index file */
	search_inside_location("index");
	if (iter != header_data->it->second.cend())
	{
		header_data->requested_resource += iter->second;
		serve_the_file();
	}
	else if (search_inside_location("auto_indexing") == 1)
	{
		/* if auto index off */
		if (iter->second == "off")
			header_data->res_status = 403;
		/* if auto index on */
		else if (iter->second == "on")
		{
			generateAutoIndex(header_data->requested_resource);
			auto_index = true;
			header_data->res_status = 200;
		}
	}
	else
	{
		header_data->requested_resource += server_data->index;
		serve_the_file();
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
	std::cout << "[GET]" << std::endl;
	int			status;
	int			ind;

	ind = header_data->new_uri.size() - 1;
	/* requesting a directory */
	if (header_data->is_dir == true)
	{
		std::cout << "[" << header_data->requested_resource << "]" << " is a directory" << std::endl;
		/* requesting a directory without '/' at the end */
		if (header_data->new_uri[ind] != '/')
		{
			std::cout << "[" << header_data->requested_resource << "]" << " is redirected" << std::endl;
			header_data->res_status = 301;
			header_data->new_uri += "/";
			return ;
		}
		requested_resource_is_dir();
	}
	/* requesting a file */
	else
	{
		std::cout << "[" << header_data->requested_resource << "]" << " is a file" << std::endl;
		serve_the_file();
	}
}

/* POST method */
void	response::Post_method()
{
	int			status;
	std::cout << "[POST]" << std::endl;
	if (header_data->res_status == 301)
	{
		header_data->new_uri += "/";
		return ;
	}
	if (header_data->_is_cgi == true)
		handle_cgi();
}

int	response::delete_the_file()
{
	/* first let's check if this file is exist */
	if (access(header_data->requested_resource.c_str(), F_OK) < 0) 
	{
		header_data->res_status = 404;
		return (404);
	}
	/* than let's check if this file has the permission */
	if (access(header_data->requested_resource.c_str(), W_OK) < 0) 
	{
		header_data->res_status = 403;
		return (403);
	}	
	/* remove the file if the return 0 that mean it successed .. */
	if (std::remove(header_data->requested_resource.c_str()) == 0)
	{
		header_data->res_status = 204;
		return(204);
	}
	/* can't remove this file ... premission denied! */
	header_data->res_status = 403;
	return (403);
}

void	response::delete_path( const char *path)
{
	if (access(path, W_OK) == 0)
	{
		if (remove(path) == 0)
			s204++;
	}
	else
		s403++;
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
			status = Client::is_file_or_directory(str.c_str(), header_data);
		}
		/* first remove all files */
		if (status == 0 && CMP(dp->d_name))
		{	
			delete_path(str.c_str());
		}
		/* than remove all subdirectories recessively */
		else if (status == 1 && CMP(dp->d_name)) 
		{
			delete_dir(str.c_str());
			delete_path(str.c_str());
		}
	}
	closedir(dir);
	return 0;
}



/* Delete method */
void	response:: Delete_method()
{
	int			status;
	int			size;

	size = header_data->requested_resource.size();
	{
		
	}
	status = Client::is_file_or_directory(header_data->requested_resource.c_str(), header_data);
	if (header_data->is_dir == true)
	{
		if (header_data->uri[header_data->uri.size() - 1] != '/')
		{
			header_data->res_status = 409;
			return ;
		}
		header_data->requested_resource.resize(size - 1);
		delete_dir(header_data->requested_resource.c_str());
		if (s204 && !s403)
			header_data->res_status = 204;
		else if (s204 >= s403)
			header_data->res_status = 207;
		else if (s403 && !s204)
			header_data->res_status = 403;
		else if (s403 > s204)
			header_data->res_status = 500;
	}
	if (status == 0)
		delete_the_file();
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

void	response::get_res_status( std::string res )
{

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
	struct stat s;
	
	if (default_response == false)
	{
		// if (header_data->_is_cgi == true)
		// {
		// 	if (stat(cgi_output.c_str(), &s) == 0)
		// 	{
		// 		Content_Length += std::to_string(s.st_size - cgi_body_pos);
    	//     	Content_Length += "\r\n\r\n";
		// 		return Content_Length;
		// 	}
		// }
    	if (stat(header_data->requested_resource.c_str(), &s) == 0)
		{
			content_length = s.st_size;
			Content_Length += std::to_string(content_length);
    	    Content_Length += "\r\n\r\n";
			return Content_Length;
    	}
	}
	if (default_response == true)
		content_length = response_content.size();
	return (Content_Length + std::to_string(content_length) + "\r\n\r\n");
}

void	response::Pages()
{
	if (header_data->method == "GET" && header_data->res_status == 200)
		return ;
	else if (header_data->method == "POST" && header_data->_is_cgi && header_data->res_status == 201)
		return ;
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
	if (header_data->_is_cgi == true)
	{
		iter = cgi_header.find("location");
		if (iter != cgi_header.end())
		{
			Location += iter->second;
			Location += "\r\n";
			return (Location);
		}
	}
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

/* ============================================================================== */

void	response::Parse_Line( std::string line )
{
	std::string key(line);
	std::string value(line);
	int 		find;
	bool		status;

	find = line.find_first_of(":");
	if (find != std::string::npos)
	{
		key.resize(find);
		value.erase(0, find + 1);
		status = cgi_header.insert(std::make_pair(key, value)).second;
		if (status == false)
		{
			cgi_header.erase(key);
			std::cout<< "KEY : " << key << "VALUE : " << value << std::endl;
			cgi_header.insert(std::make_pair(key, value));
		}
	}
}
/*
	Start_line
	HTTP/1.1 200 OK || Status: 200 OK
	
	Content-Type: text/html; charset=UTF-8
	Content-Length: 1024
	Cache-Control: no-cache
	Location: https://www.example.com
	Set-Cookie: session_id=abc123; path=/; HttpOnly
*/

std::string toLowerCase(const std::string& str) {
    std::string result;
    for (std::size_t i = 0; i < str.length(); ++i) {
        result += std::tolower(str[i]);
    }
    return result;
}

void	response::Parse_cgi_header( std::string cgi_header )
{
	int 	find;
	std::string res;
	find = cgi_header.find("\r\n\r\n");
	cgi_body_pos = 0;
	if (find != std::string::npos)
	{
		cgi_body_pos = find + 4;
		cgi_header.resize(find + 2);
		find = cgi_header.find_first_of("\r\n");
		// cgi_body_pos += find;
		cgi_start_line = cgi_header;
		cgi_start_line.resize(find);
		while (find != std::string::npos)
		{
			res = cgi_header;
			res.resize(find);
			cgi_header.erase(0, find + 2);
			res = toLowerCase(res);
			Parse_Line(res);
			std::cout << "LINE : " << std::endl;
			find = cgi_header.find_first_of("\r\n");
			// cgi_body_pos += find;
		}
	}
	/* start Line */
	/* Content-Type */
	/* Content-Length */
	/* Location */
	/* Set-Cookie: */
}

void	response::Put_Header(const std::string &key, const std::string &value)
{
	struct stat s;
	long	result;

	if (key != "status")
	{
		header += key + ": ";
		header += value + "\r\n";
	}
}

void	response::Process_Cgi_Header()
{
	std::map<std::string, std::string>::iterator end = cgi_header.end();
	int	find;
	/* start line */
	header += HTTP_V;
	find = cgi_start_line.find("HTTP/1.1");
	if (find != std::string::npos)
	{
		cgi_start_line.erase(find, cgi_start_line.find(" ", find + 1));
		header_data->res_status = atoi(cgi_start_line.c_str());
		header += cgi_start_line;
		if (VALID_STATUS(header_data->res_status) == false)
			header_data->res_status = 502;
		/* ... throw execption ... */
	}
	else if ((iter = cgi_header.find("status")) != end)
	{
		response_content = iter->second;
		header += response_content;
		header_data->res_status = atoi(iter->second.c_str());
		if (VALID_STATUS(header_data->res_status) == false)
			header_data->res_status = 502;
	}
	else
		header += "200 OK";
	header += "\r\n";
	iter = cgi_header.begin();
	
	while (iter != end)
	{
		std::cout << iter->first << ": " << iter->second << std::endl;
		Put_Header(iter->first, iter->second);
		iter++;
	}
	header += "\r\n";
}

void	response::handle_cgi_header( )
{
	std::string line;
	requested_file.open(cgi_output, std::ios::binary);
	if (requested_file.is_open())
	{
		memset(buffer, 0, BUFFER_SIZE);
		requested_file.read(buffer, BUFFER_SIZE);
		std::streamsize size = requested_file.gcount();
		Parse_cgi_header(buffer);
	}
	requested_file.close();
	Process_Cgi_Header();
}

/* ============================================================================== */

	/* HTTP/1.1 200 OK\r\n                      */
	/* Date: Wed, 11 May 2023 12:00:00 GMT\r\n  */
	/* Server: Apache/2.4.38 (Unix)\r\n         */
	/* Content-Type: text/html\r\n              */
	/* Content-Length: 1234\r\n\r\n             */
void response::create_header()
{
	if (header_data->_is_cgi == true)
	{
		handle_cgi_header();
		if (FAILED(header_data->res_status) == true)
			Pages();
		return ;
	}
	header = get_start_line();
	header += Get_Date();
	header += SERVER;
	
	if (header_data->res_status >= 300 && header_data->res_status <= 308)
		header += get_Location();
	if (header_data->res_status == 405)
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