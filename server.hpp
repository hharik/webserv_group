/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:49:46 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/21 16:24:37 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include "parsing.hpp"

class Server
{
	parsing					parser;
	std::vector<Socket*>	servers;
	User_data				*user_data;
	int						kq;
	long					events_nbr;
	unsigned int			events_size;
	bool					new_event;
	struct 	kevent			*events;
	struct 	kevent			eventToRemove[2];

public:
	Server(const std::string&);
	~Server();

	void	Create_queue_object();
	void	Create_http_servers();
	void	Wait_for_incoming_events();
	void	Destroy_clients();
	void	Destroy_resource( int );
};

#endif /* SERVER_HPP */
