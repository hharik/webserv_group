/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 16:39:08 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/19 09:42:26 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "parsing.hpp"
#include "user_data.hpp"


data_header::data_header() : Content_Length(-2), res_status(0), is_redirect(false)
{
	read_p = true;
	write_p = true;
	exec_p = true;
	is_dir = false;
}

Client::Client( const data_serv *dptr, Socket *Pbase ): Base(Pbase)
{
	header_data = new data_header();
	user_data = new User_data();
	client_response = new response(dptr, header_data);
	client_request = new request(dptr, header_data);
}

Client::~Client()
{
	delete client_request;
	delete client_response;
	delete header_data;
}

void	Client::client_connection( int server_socket )
{
	memset(&client, 0, sizeof(client));
	len = sizeof(client);
	fd = accept(server_socket, &client, &len);
	if (fd < 0)
	{
		client_response->set_eof(true);
		perror("Client : Accept ");
	}
	else if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		client_response->set_eof(true);
		perror("Client : Fcntl ");
		return ;
	}
}

void	Client::attach_client_socket( int kq )
{
	user_data->set_status(false);
	user_data->set_client(this);
	user_data->set_socket(Base);
	EV_SET(&client_event[0], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, user_data);
	EV_SET(&client_event[1], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, user_data);
	if (kevent(kq, client_event, 2, NULL, 0, NULL) < 0)
	{
		perror("Client : Kevent ");
		client_response->set_eof(true);
	}
}
 
void	Client::send_the_response()
{
	if (header_data->res_status > 0 || client_request->end_of_file == true)
	{
		client_response->response_handler(fd);
	}
}

int	Client::read_from_socket()
{
	memset(buffer, 0, BUFFER_SIZE);
	nbytes = recv(fd, buffer, BUFFER_SIZE, 0);
	if ( nbytes > 0 )
	{
		request_buffer.append(buffer, nbytes);
		std::cout << buffer << std::endl;
	}
	else
	{
		perror("Client : recv ");
		client_response->set_eof(true);
		return (-1);
	}
	if (client_request->end_of_file == false && header_data->res_status == 0)
		client_request->parse(request_buffer);
	return  (1);
}

bool	Client::eof()
{
	return client_response->IsEndOfFile();
}

int	Client::get_fd()
{
	return fd;
}

int Client::is_file_or_directory(const char *str, data_header *header_ptr)
{
	struct	stat	spath;

	if (stat(str, &spath) == 0)
	{
		if (S_ISREG(spath.st_mode))
			return (0);
		else if (S_ISDIR(spath.st_mode))
		{
			if (access(str, W_OK) != 0)
				header_ptr->write_p = false;
			if (access(str, R_OK) != 0)
				header_ptr->read_p = false;
			if (access(str, X_OK) != 0)
				header_ptr->exec_p = false;
			return (1);
		}
	}
	return (-1);
}