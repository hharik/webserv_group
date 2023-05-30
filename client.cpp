/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 16:39:08 by ajemraou          #+#    #+#             */
/*   Updated: 2023/05/28 17:10:29 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include <cstdlib>
// this will be removed ... 
// #include "server.hpp"
// #include "socket.hpp"
// #include "user_data.hpp"

Client::Client()
{
	user_data = new User_data();
	// is_reading = false;
}

void	Client::client_connection( int server_socket )
{
	memset(&client, 0, sizeof(client));
	len = sizeof(client);
	std::cout << "server_socket ... " << server_socket <<  std::endl;
	fd = accept(server_socket, &client, &len);
	std::cout << "fd : " << fd << std::endl;
	if (fd < 0)
	{
		perror("client:accept");
	}
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		perror("client:fcntl");
		exit(EXIT_FAILURE);
	}
}

void	Client::attach_client_socket( int kq )
{
	user_data->set_status(false);
	user_data->set_client(this);
	EV_SET(&client_event[0], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, user_data);
	EV_SET(&client_event[1], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, user_data);
	if (kevent(kq, client_event, 2, NULL, 0, NULL) < 0)
	{
		perror("kevent");
		exit(EXIT_FAILURE);
	}
}

void	Client::read_from_socket()
{
	memset(buffer, 0, BUFFER_SIZE);
	nbytes = recv(fd, buffer, BUFFER_SIZE, 0);
	if ( nbytes > 0 )
	{
		client_request.append(buffer, nbytes);
		// client_response.
	}
	else
	{
		perror("recv");
		exit(EXIT_FAILURE);
	}
}
