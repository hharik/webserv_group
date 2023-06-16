/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 18:03:44 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/16 20:23:31 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"
#include "client.hpp"
#include "parsing.hpp"
#include "user_data.hpp"

Socket::Socket()
{
	user_data = new User_data();
	server_data = new data_serv();
	reuseaddr_opt = 1;
	events_nbr = 0;
	clients_ind = 0;
	new_client = false;
	status = 0;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
}

Socket::~Socket()
{
	delete user_data;
	delete server_data;
}

int	Socket::Create_the_socket( )
{
	status = getaddrinfo(server_data->server_name.c_str(), server_data->port.c_str() , &hints, &result); //get addr info based on the hint struct
	
	if (status)
	{
		std::cerr << "Server :" << gai_strerror(status) << std::endl;
		return -1;
	}
 	// Create the Server socket
	sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (sockfd < 0)
	{
        perror("Server : Socket ");
		return -1;
	}
	// Set socket options 
	status = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_opt, sizeof(reuseaddr_opt));
    if (status < 0)
	{
		perror("Server : Setsockopt ");
        return -1;
    }
	// use non-blocking socket
	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
	{
		perror("Server : Fcntl ");
		return -1;
	}
	// bind the socket 
	status = bind(sockfd, result->ai_addr, sizeof(*(result->ai_addr)));
	if (status < 0)
	{
		perror("Server : Bind ");
		return -1;
	}
	status = listen(sockfd, BACKLOG);
	if (status < 0)
	{
		perror("Server : Bind ");
		return -1;
	}
	return sockfd;
}

void	Socket::attach_server_socket( int kq )
{

	user_data->set_status(true);
	user_data->set_socket(this);
	EV_SET(&server_event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, user_data);

    /* Register the server event with the kqueue */
    status = kevent(kq, &server_event, 1, NULL, 0, NULL);
	if (status < 0)
	{
        perror("Server : kevent ");
        exit(EXIT_FAILURE);
    }
}

void	Socket::Accept_new_connection( int kq )
{
	clients.push_back(new Client(server_data, this));
	/* accept new connction */
	clients_ind = clients.size() - 1;
	clients[clients_ind]->client_connection(sockfd);
	clients[clients_ind]->attach_client_socket(kq);
}

void	Socket::Destruct_client(  )
{
	int	index;

	index = 0;
	while (index < clients.size())
	{
		if (clients[index]->eof() == true)
		{
			delete clients[index];
			clients.erase(clients.begin() + index);
		}
		else
		{
			index++;
		}
	}
}

void	Socket::set_server_data(data_serv &data)
{
	*server_data = data;
}
