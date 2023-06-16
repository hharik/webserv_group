/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 16:39:08 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/16 18:11:57 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "parsing.hpp"
#include "user_data.hpp"

Client::Client( const data_serv *dptr, Socket *Pbase ):server_data(dptr), Base(Pbase), header_data(new data_header)
{
	user_data = new User_data();
	client_response = new response(dptr, header_data);
	client_request = new request(dptr, header_data);
	// is_finish = false;
}

Client::~Client()
{
	// delete user_data;
	delete client_request;
	delete client_response;
	delete header_data;
}

void	Client::client_connection( int server_socket )
{
	memset(&client, 0, sizeof(client));
	len = sizeof(client);
	// std::cout << "server_socket ... " << server_socket <<  std::endl;]
	fd = accept(server_socket, &client, &len);
	// std::cout << "fd : " << fd << std::endl;
	if (fd < 0)
	{
		perror("client:accept");
	}
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		client_response->set_eof(true);
		perror("client:fcntl");
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
		perror("kevent");
		exit(EXIT_FAILURE);
	}
}

void	Client::send_the_response()
{
		if (header_data->res_status > 0 || client_request->end_of_file == true)
		{
			client_response->response_handler(fd);
		}
}

void	Client::read_from_socket()
{
	memset(buffer, 0, BUFFER_SIZE);
	nbytes = recv(fd, buffer, BUFFER_SIZE, 0);
	if ( nbytes > 0 )
		request_buffer.append(buffer, nbytes);
	else
	{
		perror("recv_client : ");
		client_response->set_eof(true);
		return ;
	}
	if (client_request->end_of_file == false && header_data->res_status == 0)
		client_request->parse(request_buffer);
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
			if ((spath.st_mode & S_IWUSR) == 0)
				header_ptr->write_p = false;
			if ((spath.st_mode & S_IRUSR) == 0)
				header_ptr->read_p = false;
			if ((spath.st_mode & S_IXUSR) == 0)
				header_ptr->exec_p = false;
			return (1);
		}
	}
	return (-1);
}