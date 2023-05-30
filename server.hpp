/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:49:46 by ajemraou          #+#    #+#             */
/*   Updated: 2023/05/28 17:16:54 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

// #pragma once

// #include <iostream>
// #include <sys/_types/_socklen_t.h>
// #include <sys/types.h>
// #include <sys/event.h>
// #include <sys/time.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <fcntl.h>
// #include <unistd.h>
// #include <vector>
// /* hamid includes */
// # include <map>
// # include <stack>
// # include <fstream>

#include "parsing.hpp"
#include "socket.hpp"
#include "user_data.hpp"
#include <vector>

// macros
#define BACKLOG 10
#define BUFFER_SIZE 1024

class parsing;

class Server
{
	User_data			*user_data;
	int 				kq;
	int 				status;
	unsigned int		events_nbr;
	unsigned int		events_size;
	bool				new_event;

	std::vector<Socket*> servers;
	parsing		parser;	
	// std::vector<struct kevent*> vevents;
	struct 	kevent	*events;
	// std::vector<struct kevent> vevents;
	// std::vector<>
	// server events
	// // struct 	kevent	*events;
	// static variabls
public:
	Server(const std::string&);
	~Server(){}

	void	Create_queue_object();
	void	Create_http_servers();
	void	Wait_for_incoming_events();

	// void	attach_server_to_queue( int );
	// static member functions
	// static int	get_queue_fd();
};

#endif /* SERVER_HPP */
