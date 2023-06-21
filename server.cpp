/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 15:06:59 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/21 07:59:35 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "user_data.hpp"
#include "server.hpp"
#include "socket.hpp"

Server::Server(const std::string &config_file):parser(config_file), servers()
{
	std::vector<data_serv>::iterator it;
	std::vector<data_serv>::iterator end;
	parser.readAndParse();
	parser.mime();
	events_nbr = 0;
	events_size = 0;
	new_event = false;
	Create_queue_object();
	it = parser.servers_data.begin();
	end = parser.servers_data.end();
	for (int i = 0;it != end;it++ , i++)
	{
		servers.push_back(new Socket());
		servers[i]->set_server_data(*it);
		servers[i]->Create_the_socket();
		servers[i]->attach_server_socket( kq );
		events_size++;
	}
	if (events_size > 0)
	{
		events = new struct kevent [events_size];
	}
}

Server::~Server()
{
	int	size;
	int	i;

	size = servers.size();
	i = 0;
	while(i < size)
	{
		delete servers[i];
		i++;
	}
	delete [] events;
}

void	Server::Destroy_clients()
{
	for( int i = 0; i < events_nbr; i++)
	{
		if (user_data->get_is_terminated() == true)
		{
			user_data = (User_data*)events[i].udata;
			user_data->get_client()->close_fd();
			user_data->get_socket()->Destruct_client( user_data->get_client() );
			events_size--;
			delete user_data;
		}
	}
	std::cout << "EVENT SISZE : " << events_size << std::endl; 
}



void	Server::Create_http_servers()
{
	int 	status;

	while( true )
	{
		
		Wait_for_incoming_events();
		for(unsigned int i = 0; i < events_nbr; i++)
		{
			user_data = (User_data*)events[i].udata;
			/* new client need to establish the connection with this server through this socket */
			if (events[i].filter == EVFILT_READ && user_data->get_status() == true)
			{
				status = user_data->get_socket()->Accept_new_connection( kq );
				if (status == -1)
				{
					std::cout << "SOCKET  ERROR " << std::endl;
					break;
				}
				events_size++;
				new_event = true;
			}
			else if (user_data->get_status() == false)
			{
				if (events[i].filter == EVFILT_READ)
				{
					status = user_data->get_client()->read_from_socket();
				}
				/* send the response to the client */
				if (status == 1 && events[i].filter == EVFILT_WRITE)
				{
					user_data->get_client()->send_the_response();
				}
		
			 	if (status == -1 || user_data->get_is_terminated() == true)
				{

					EV_SET(&eventToRemove[0], events[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
					EV_SET(&eventToRemove[1], events[i].ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
					status = kevent(kq, eventToRemove, 2, NULL, 0, NULL);
					if (status < 0)
					{
						std::cout << "ERROR" << std::endl;
						// exit(0);
						// break ;
						
					}
					user_data->get_socket()->Destruct_client( user_data->get_client() );
					events_size--;
					events_nbr--;
					new_event = true;
					close(events[i].ident);
					delete user_data;
					user_data = NULL;
					break ;
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
		perror("Http : Server ");
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
		perror("Http : Server ");
		exit(EXIT_FAILURE);
	}
}