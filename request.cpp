

#include "client.hpp"
#include "parsing.hpp"

request::request( const data_serv *dptr, data_header *hptr ) : server_data(dptr), d_header(hptr), rsize(0) , size(0), chunked_size(-2), end_of_file(false)
{
	d_header->_is_cgi = false;
}

request::~request()
{
	
}

const std::string	request::get_extension( const std::string& target )
{
	std::string	result(target);
	int	ind;

	ind = target.find_last_of('.');
	result.erase(0, ind);
	return (result);
}


int	request::treat_target_resource( std::string path, std::string to_append ,std::string &result )
{
	std::string	rest(to_append);
	size_t		size;

	d_header->iter = d_header->it->second.find(path);
	if (d_header->iter != d_header->it->second.cend())
	{
		size = d_header->it->first.size();
		if (d_header->it->first[size - 1] != '/')
			size++;
		rest.erase(0, size);
		result = d_header->iter->second + rest;
		return (1);
	}
	return (0);
}

int	request::generate_name()
{
	/*error u dont generate names when uploading to me files 
	only gives directory in requested_resource */
	int status;
	int	ind;

	new_name = d_header->requested_resource;
	ind = d_header->new_uri.size();
	if (d_header->_is_cgi == false)
	{
		if (d_header->is_dir == true)
		{
			if (d_header->new_uri[ind - 1] != '/')
			{
				d_header->res_status = 301;
				return (0);
			}
			d_header->requested_resource += time_date() +  "." + parsing::mime_type.find(d_header->Content_type)->second;
		}
		else
		{
			ind = new_name.find_last_of(".");
			if (ind != std::string::npos)
				new_name.resize(ind);
			new_name += "." + parsing::mime_type.find(d_header->Content_type)->second;
			status = rename(d_header->requested_resource.c_str(), new_name.c_str());
			if (status == 0)
				d_header->requested_resource = new_name;
		}
	}
	else if (d_header->_is_cgi == true)
	{
		d_header->requested_resource += "/tmp/" + time_date();
	}
	return (1);
}

int		request::path_is_exist( std::string &path )
{
	int		ind;

	ind = path.size() - 1;
	if (access(path.c_str(), F_OK))
	{
		if (path[ind] == '/')
			path.resize(ind);
		if (access(path.c_str(), F_OK))
		{
			d_header->res_status = 404;
			return (0);
		}
	}
	if (access(path.c_str(), W_OK))
	{
		d_header->write_p = false;
	}
	if (access(path.c_str(), R_OK))
	{
		d_header->read_p = false;
	}
	if (access(path.c_str(), X_OK))
	{
		d_header->exec_p = false;
	}
	return (1);
}

void	request::default_root( std::string to_append, std::string &result )
{
	std::string	rest(server_data->root_dir);
	size_t		size;

	size = server_data->root_dir.size();
	if (rest[size - 1] == '/')
		size--;
	rest.resize(size);
	result = rest + to_append;
}

void	request::handle_DeleteMethod()
{
	int		status_code;
	/* Check if this location has a root */
	status_code = treat_target_resource("root", d_header->new_uri, d_header->requested_resource);
	/* If the location does not have a root, it will work with the default root */
	if (status_code == 0)
	{
		default_root(d_header->new_uri, d_header->requested_resource);
	}
	if (access(d_header->requested_resource.c_str(), F_OK) == 0)
	{
		status_code = Client::is_file_or_directory(d_header->requested_resource.c_str(), d_header);
		/* Requested Resource is a Directory */
		if (status_code == 1)
		{
			d_header->is_dir = true;
			/* Check if this directory has read and execute permissions */
			if (d_header->read_p == false)
			{
				d_header->res_status = 403;
			}
		}
		/* Requested Resource is not a Directory */
		else
		{
			status_code = path_is_exist(d_header->requested_resource);
			/* Check if this file has write permission */
			if (d_header->write_p == false)
			{
				d_header->res_status = 403;
			}
		}

	}
	else
	{
		d_header->res_status = 404;
	}
}

void	request::handle_GetMethod()
{
	int		status_code;

	/* Check if this location has a root */
	status_code = treat_target_resource("root", d_header->new_uri, d_header->requested_resource);
	/* If the location does not have a root, it will work with the default root */
	if (status_code == 0)
	{
		default_root(d_header->new_uri, d_header->requested_resource);
	}
	if (access(d_header->requested_resource.c_str(), F_OK) == 0)
	{
		status_code = Client::is_file_or_directory(d_header->requested_resource.c_str(), d_header);
		/* Requested Resource is a Directory */
		if (status_code == 1)
		{
			d_header->is_dir = true;
			/* Check if this directory has execution and read permissions */
			if (d_header->exec_p == false || d_header->read_p == false)
			{
				d_header->res_status = 403;
			}
		}
		/* Requested Resource is not a Directory */
		else
		{
			status_code = path_is_exist(d_header->requested_resource);
			/* Check if this file has read permission */
			if (d_header->read_p == false)
			{
				d_header->res_status = 403;
			}
			/* Check if this file can be passed as an input to the CGI */
			status_code = treat_target_resource("cgi " + get_extension(d_header->requested_resource), "", d_header->cgi_path);
			/* needs check if this input file has the permission  */
			if (status_code == 1)
			{
				d_header->_is_cgi = true;
			}
		}

	}
	else
	{
		d_header->res_status = 404;
	}
}

void	request::ProvideToUpload( std::string path)
{
	int status;

	std::cout << "Uploading " << path << std::endl;
	if (access(path.c_str(), F_OK))
	{
		update_the_uri(path);
		ProvideToUpload(path);
		if (access(path.c_str(), F_OK))
		{
			status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			if (status == -1)
			{
				d_header->res_status = 403;
				return ;
			}
		}
	}
	d_header->res_status = 0;
}

void request::handle_PostMethod()
{
	int		status_code;

	/* Check if this location has an upload */
	status_code = treat_target_resource("upload", d_header->new_uri, d_header->requested_resource);
	if (status_code == 1 && access(d_header->requested_resource.c_str(), F_OK) == 0)
	{
		status_code = Client::is_file_or_directory(d_header->requested_resource.c_str(), d_header);
		/* Requested Resource is a Directory */
		if (status_code == 1)
		{
			d_header->is_dir = true;
			/* Check if this directory has execution and write permission */
			if (d_header->exec_p == false || d_header->write_p == false)
			{
				d_header->res_status = 403;
			}
		}
		/* Requested Resource is not a Directory */
		else
		{
			status_code = path_is_exist(d_header->requested_resource);
			/* Check if this file has write permission */
			if (d_header->write_p == false)
			{
				d_header->res_status = 403;
			}
		}
	}
	/* Check if this file can be passed as an input to the CGI */
	else
	{
		status_code = treat_target_resource("cgi " + get_extension(d_header->new_uri), "", d_header->cgi_path);
		/* This file can be passed as an input to the CGI  */
		if (status_code == 1)
		{
			/* Append this to the root, then process the result */
			status_code = treat_target_resource("root", d_header->new_uri, d_header->cgi_script);
			if (status_code == 0)
				default_root(d_header->cgi_script, d_header->new_uri);
			/* Check if this file has the required permission to be passed as an input to the CGI */
			/* This function may cause an error (something like a missed or extra '/' at the end in the cgi-script) */
			status_code = path_is_exist(d_header->cgi_script);
			/* Check if the file exists */
			if (status_code == 0)
			{
				d_header->res_status = 404;
			}
			/*  Check if the file has read permission */
			else if (d_header->read_p == false)
			{
				d_header->res_status = 403;
			}
			/* This file can be passed as an input to the CGI  */
			else 
			{
				d_header->_is_cgi = true;
			}
		}
		else
		{
			std::cout << "FORBIDEN 88888 * ** ** **  * " << std::endl;
			d_header->res_status = 403;
		}
	}
}


int	request::get_requested_resource()
{
	if (d_header->method == "GET")
		handle_GetMethod();
	else if (d_header->method == "POST")
		handle_PostMethod();
	else if (d_header->method == "DELETE")
		handle_DeleteMethod();
	return (d_header->res_status);
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

	strftime(date_string, 50, "%B-%d-%Y-%H-%M-%S", curr_tm);
	return std::string(date_string);
}




void	request::save_chunk_improve(std::string &body) 
{
	std::string::iterator first = body.begin();
	std::string::iterator last;
	std::string to_delete = "\r\n";
	if (file_obj.is_open() == false)
	{
		// body.erase(0, body.find(to_delete));
		std::cout <<  "here is the file to out " <<  d_header->requested_resource << std::endl;
		file_obj.open(d_header->requested_resource, std::fstream::out | std::fstream::trunc | std::fstream::binary);
	}
	std::stringstream to_hex;

	while (chunked_size != 0)
	{
		if (chunked_size == -2)
		{
			/*parce hexa */ 
			first = std::search(body.begin(), body.end(), to_delete.begin(), to_delete.end());

			std::string::iterator last = std::search(first + 1, body.end(), to_delete.begin(), to_delete.end());
			std::string test(first, last);
			// std::cout << "*" << test <<  "* "<<  std::endl;
			if (test == "\r\n0")
			{
				end_of_file = true;
				d_header->res_status = 201;
			}
			to_hex << test;
			to_hex >> std::hex >> chunked_size;
			body.erase(0, test.length() + 2);
		}
		if (chunked_size >= body.size())
		{
			file_obj << body;
			size += body.size();
			chunked_size -= body.size();
			body.clear();
			if (chunked_size == 0)
				chunked_size = -2;
			break ;
		}
		else {
			file_obj << body.substr(0, chunked_size);
			size += chunked_size;
			body.erase(0, chunked_size);
			chunked_size = -2;
			if (body.size() <= 2 || body.find("\r\n", 2) == std::string::npos)
				break;
		}
	}
	if (end_of_file == true)
	{
		file_obj.close();
	}
}

void request::save_binary(std::string &header)
{
	if (file_obj.is_open() == false)
	{
		file_obj.open(d_header->requested_resource, std::fstream::out | std::fstream::binary | std::fstream::trunc);
		header.erase(0, 2);
	}
	if (file_obj.is_open() == true)
	{
		size += header.size();
		file_obj << header;
		header.clear();
	}
	// file_obj.close();
	if (size == d_header->Content_Length)
	{
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
				if (d_header->method == "POST" && server_data->max_body_size < d_header->Content_Length)
				{
					d_header->res_status = 413;
					return ;
				}

			}
			if (buffer.find("Content-Type:") != std::string::npos)
			{
				d_header->Content_type = buffer.substr(pos + 2);
				d_header->Content_type.pop_back();
				if (d_header->Content_type.find("multipart/form-data") != std::string::npos)
				{
					d_header->boundary = buffer.substr(buffer.find_last_of("boundary=") + 1);
					d_header->boundary.pop_back();
				}
				if (d_header->method == "POST" && d_header->Content_type.empty() == true)
				{
					d_header->res_status = 415;
					return ;
				}
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
			if (buffer.find("Cookie:") != std::string::npos)
			{
				d_header->Cookies = (buffer.substr(pos + 2));
				d_header->Cookies.pop_back();
			}
			if (buffer.size() == 1 && buffer == "\r")
			{
				break ;
			}
		}
		if (header.find("\r\n\r\n") != std::string::npos)
			header.erase(0, header.find("\r\n\r\n") + 2);
		else
		{
			d_header->res_status = 431;
			return ;
		}
		/* check if the requested uri exist in locations block */
		if (find_required_location() < 0)
		{
		std::cout << "STATUS_CODE 1 : " << d_header->res_status << std::endl;
			return ;
		}
		/* requested uri is exist */
		/* if this location have a redirection */
		else if (check_for_redirection() < 0)
		{
		std::cout << "STATUS_CODE 2 : " << d_header->res_status << std::endl;

			return ;
		}
		/* if this method is allowed in this location */
		else if (allowed_methods() < 0)
		{
		std::cout << "STATUS_CODE 3 : " << d_header->res_status << std::endl;

			return ;
		}
		else if (get_requested_resource() != 0)
		{
			std::cout << "STATUS_CODE 4 : " << d_header->res_status << std::endl;
			return ;
		}
		/* generate the name of the file if hte method is POST */
		if (d_header->method == "POST" && generate_name() == 0)
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
	if (d_header->method == "POST") 
	{
	
		if (d_header->transfer_encoding.empty() == true && d_header->Content_Length ==  -2)
		{
			d_header->res_status = 400;
			return;
		}
		if (d_header->transfer_encoding.empty() == false)
		{
			save_chunk_improve(header);
			if (size > server_data->max_body_size)
			{
				d_header->res_status = 413;
				return ;
			}
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
	if (std::string::npos != find && !find && uri.size() > 1)
	{
		uri = "/";
		return (1);
	}
	if (std::string::npos != find)
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
	result = 1;
	updated_uri = d_header->new_uri;
	while (result)
	{
		d_header->it = server_data->location.find(updated_uri);
		if (d_header->it != server_data->location.cend())
			return (0);
		d_header->it = server_data->location.find(updated_uri + "/");
		if (d_header->it != server_data->location.cend())
			return (0);
		result = update_the_uri(updated_uri);
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
