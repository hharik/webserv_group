/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:06:59 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/06 09:59:54 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "user_data.hpp"
#include "server.hpp"
#include "socket.hpp"
#include <sys/event.h>
#include <unistd.h>

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
		servers[i]->set_server_data(*it);
		servers[i]->Create_the_socket();
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
			/* new client need to establish the connection with this server through this socket */
			if (events[i].filter == EVFILT_READ && user_data->get_status() == true)
			{
				std::cout << "NeW ClIent ......... " << std::endl;
				user_data->get_socket()->Accept_new_connection( kq );
				events_size++;
				new_event = true;
			}
			/* client request */
			else if (events[i].filter == EVFILT_READ)
			{
				user_data->get_client()->read_from_socket();
			}
			if (events[i].filter == EVFILT_WRITE)
			{
				user_data->get_client()->send_the_response();
				if (user_data->get_client()->eof() == true)
				{
					close(user_data->get_client()->get_fd());
					user_data->get_socket()->Destruct_client(user_data->get_client()->Get_client_inedex());
					events_size--;
					new_event = true;
				}
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
		std::cout << "events_size : " << events_size << std::endl;
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