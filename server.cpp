/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:06:59 by ajemraou          #+#    #+#             */
/*   Updated: 2023/05/28 16:29:26 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include "parsing.hpp"
#include "socket.hpp"
#include "client.hpp"
#include "user_data.hpp"
// #include "parsing.hpp"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/event.h>
#include <vector>
#include <list>

Server::Server(const std::string &config_file):parser(config_file), servers()
{
	parser.readAndParse();
	parser.mime();
	events_nbr = 0;
	events_size = 0;
	new_event = false;
	Create_queue_object();
	std::vector<data_serv>::iterator it;
	std::vector<data_serv>::iterator end;
	it = parser.servers_data.begin();
	end = parser.servers_data.end();
	for (int i = 0;it != end;it++ , i++)
	{
		servers.push_back(new Socket());
		std::cout << "host : " << it->server_name << std::endl;
		std::cout << "port : " << it->port << std::endl;
		servers[i]->Create_the_socket(it->server_name, it->port);
		servers[i]->attach_server_socket( kq );
		events_size++;
		// size of events
	}
	if (events_size > 0)
		events = new struct kevent [events_size];
}


void	Server::Create_http_servers()
{
	while( 1 )
	{
		Wait_for_incoming_events();
		for(int i = 0; i < events_nbr; i++)
		{
			user_data = (User_data*)events[i].udata;
			/* new client to establish the connection with this server through this socket */
			if (events[i].filter == EVFILT_READ && user_data->get_status() == true)
			{
				events_size++;
				new_event = true;
				user_data->get_socket()->Accept_new_connection( kq );
			}
			/* client request */
			else if (events[i].filter == EVFILT_READ)
			{
				user_data->get_client()->read_from_socket();
			}
		}
	}
	
}

void	Server::Create_queue_object()
{
	kq = kqueue();
	if (kq < 0)
	{
		perror("kqueue");
		exit(EXIT_FAILURE);
	}
}

void	Server::Wait_for_incoming_events()
{
	if (new_event == true)
	{
		delete [] events;
		events = new struct kevent [events_size];
		new_event = false;
	}
	events_nbr = kevent(kq, NULL, 0, events, events_size, NULL);
	if (events_nbr < 0)
	{
		std::cerr << "kevent : failed " << std::endl;
		exit(EXIT_FAILURE);
	}
}