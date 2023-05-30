/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:49:46 by ajemraou          #+#    #+#             */
/*   Updated: 2023/05/29 14:01:14 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "parsing.hpp"

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
