/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 16:39:08 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/21 16:07:43 by ajemraou         ###   ########.fr       */
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

Client::Client( const data_serv *dptr, Socket *Pbase, int cfd ): Base(Pbase), fd(cfd)
{
	user_data = new User_data();
	header_data = new data_header();
	client_response = new response(dptr, header_data);
	client_request = new request(dptr, header_data);
}

Client::~Client()
{
	delete client_request;
	delete client_response;
	delete header_data;
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
		user_data->set_is_terminated(true);
	}
}
 
void	Client::send_the_response()
{
	if (header_data->res_status > 0 || client_request->end_of_file == true)
	{
		client_response->response_handler(fd);
		if (client_response->Get_eof() == true)
			user_data->set_is_terminated(true);
	}
}

int	Client::read_from_socket()
{
	memset(buffer, 0, BUFFER_SIZE);
	nbytes = recv(fd, buffer, BUFFER_SIZE, 0);
	if ( nbytes > 0 )
	{
		request_buffer.append(buffer, nbytes);
	}
	else
	{
		perror("Client : recv ");
		user_data->set_is_terminated(true);
		return (-1);
	}
	if (client_request->end_of_file == false && header_data->res_status == 0)
	{
		client_request->parse(request_buffer);
	}
	return  (1);
}

void	Client::client_index( int index )
{
	header_data->client_index = "-" + std::to_string(index);
}

void	Client::close_fd()
{
	close(fd);
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

void	Client::SetTemporaryPath( std::string Path )
{
	header_data->TempPath = Path;
}
