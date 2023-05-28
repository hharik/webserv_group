/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/26 18:03:44 by ajemraou          #+#    #+#             */
/*   Updated: 2023/05/28 16:38:03 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "socket.hpp"
#include "client.hpp"
#include "user_data.hpp"
#include <_stdio.h>
#include <cstdlib>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/event.h>

Socket::Socket()
{
	std::cout << "default constructor" << std::endl;
	user_data = new User_data();
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

// Socket::Socket(const std::string &hostname, const std::string &servname)
// {
// 	reuseaddr_opt = 1;
// 	events_nbr = 0;
// 	clients_ind = 0;
// 	new_client = false;
// 	status = 0;
// 	memset(&hints, 0, sizeof(hints));
// 	hints.ai_family = AF_INET;// IPv4 address family
// 	hints.ai_socktype = SOCK_STREAM;// TCP socket type
// 	hints.ai_protocol = IPPROTO_TCP;// TCP protocol
// 	status = getaddrinfo(hostname.c_str(), servname.c_str() , &hints, &result); //get addr info based on the hint struct
// 	if (status < 0)
// 	{
// 		perror("getaddrinfo");
// 		exit(EXIT_FAILURE);
// 	}
// }


int	Socket::Create_the_socket( const std::string &hostname, const std::string &servname )
{
	status = getaddrinfo(hostname.c_str(), servname.c_str() , &hints, &result); //get addr info based on the hint struct
	
	if (status)
	{
		std::cerr << gai_strerror(status) << std::endl;
		// perror("getaddrinfo");
		exit(EXIT_FAILURE);
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
	std::cout << "attch_server_socket . . fd : " << sockfd << std::endl;
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
	// accept new connction
	clients.push_back(new Client());
	std::cout << "Accept_new_connection_client index : " << clients_ind << std::endl;
	clients[clients_ind]->client_connection(sockfd);
	clients[clients_ind]->attach_client_socket(kq);
	clients_ind++;
}

// void	Socket::multiplexing( int kq )
// {
// 	std::cout << "multiplexing" << std::endl;
// 	attach_server_socket( kq );
// 	while ( 1 )
// 	{
// 		Wait_for_incoming_events( kq );
// 		for (int i = 0; i < events_nbr; i++)
// 		{
// 			// if (events[i].ident == (unsigned long)sockfd && events[i].filter == EVFILT_READ)
// 			// {
// 			// 	std::cout << "new_Client added ... " << std::endl;
// 			// 	// accept new connction
// 			// 	Accept_new_connection( kq );
// 			// 	new_client = true;
// 			// }
// 			// else 
// 			// {
// 			// 	client_ptr = (Client*)events[i].udata;
// 			// 	if (events[i].filter == EVFILT_READ)
// 			// 		client_ptr->read_from_socket();
// 			// }
// 		}
// 	}
// }

// void	Socket::Wait_for_incoming_events( int kq )
// {
// 	struct kevent client_ev;
// 	if (new_client == true)
// 	{
// 		vevents.push_back(client_ev);
// 		events = vevents.data();
// 	}
// 	new_client = false;
// 	events_nbr = kevent(kq, NULL, 0, events, vevents.size(), NULL);
// 	if (events_nbr < 0)
// 	{
// 		std::cerr << "kevent : failed " << std::endl;
// 		exit(EXIT_FAILURE);
// 	}
// }
