/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 18:03:44 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/07 22:20:24 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"
#include "client.hpp"
#include "parsing.hpp"
#include "user_data.hpp"
#include <vector>

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
	hints.ai_family = AF_INET;// IPv4 address family
	hints.ai_socktype = SOCK_STREAM;// TCP socket type
	hints.ai_protocol = IPPROTO_TCP;// TCP protocol
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
		std::cerr << gai_strerror(status) << std::endl;
		return -1;
	}
 	// Create the Server socket
	sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (sockfd < 0)
	{
        perror("socket");
		exit(EXIT_FAILURE);
	}
	// Set socket options 
	status = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_opt, sizeof(reuseaddr_opt));
    if (status < 0) {
		perror("setsockopt");
        exit(EXIT_FAILURE);
    }
	// use non-blocking socket
	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
	{
		perror("server:fcntl");
		exit(EXIT_FAILURE);
	}
	// bind the socket 
	status = bind(sockfd, result->ai_addr, sizeof(*(result->ai_addr)));
	if (status < 0)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}
	status = listen(sockfd, BACKLOG);
	if (status < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	return sockfd;
}

void	Socket::attach_server_socket( int kq )
{
	// std::cout << "attch_server_socket . . fd : " << sockfd << std::endl;
	// struct kevent server_ev;
	user_data->set_status(true);
	user_data->set_socket(this);
	EV_SET(&server_event, sockfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, user_data);

    // Register the server event with the kqueue
    status = kevent(kq, &server_event, 1, NULL, 0, NULL);
	if (status < 0)
	{
        perror("kevent (serverEvent)");
        exit(EXIT_FAILURE);
    }
	// vevents.push_back(server_ev);
	// events = vevents.data();
}

void	Socket::Accept_new_connection( int kq )
{
	clients.push_back(new Client(server_data, this));
	// accept new connction
	clients_ind = clients.size() - 1;
	clients[clients_ind]->client_connection(sockfd);
	clients[clients_ind]->attach_client_socket(kq);
	// clients_ind++;
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
			index++;
	}
}

void	Socket::set_server_data(data_serv &data)
{
	*server_data = data;
}
