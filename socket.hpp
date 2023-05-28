/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:50:54 by ajemraou          #+#    #+#             */
/*   Updated: 2023/05/28 15:50:36 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

// #pragma once

// #include <iostream>
// #include <sys/_types/_socklen_t.h>
// #include <sys/types.h>
// #include <sys/event.h>
// #include <sys/time.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <vector>

// #include "server.hpp"
#include "parsing.hpp"
#include "user_data.hpp"

class Client;

class Socket
{
	// server attribute
	User_data			*user_data;
	bool				_ierver;
	int					sockfd;
	struct addrinfo		hints;
	struct addrinfo		*result;
	int					reuseaddr_opt;
	int 				status;
	int					events_nbr;
	// server events
	struct	kevent	server_event;
	// struct 	kevent	*events;
	// std::vector<struct kevent> vevents;
	// kqueue object descriptor
	// int	kq;
	// clients
	std::vector<Client*> 	clients;
	bool					new_client;
	Client					*client_ptr;
	int						clients_ind;
	// static variables
	// static	int 		kq;
public:
	Socket();
	Socket(const std::string&, const std::string&);
	
	// static member functions
	// static int	get_queue_fd();
	// member functions
	int			Create_the_socket(const std::string&, const std::string&);
	void		attach_server_socket( int );
	void		multiplexing( int );
	void		Accept_new_connection( int );
	void		Wait_for_incoming_events( int );
};

#endif /* SOCKET_HPP */