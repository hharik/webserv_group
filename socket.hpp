/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:50:54 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/07 21:43:18 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "parsing.hpp"

class Socket
{
	// server attribute
	data_serv			*server_data;
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
	~Socket();
	Socket(const std::string&, const std::string&);
	
	// static member functions
	// static int	get_queue_fd();
	// member functions
	int			Create_the_socket();
	void		attach_server_socket( int );
	void		multiplexing( int );
	void		Accept_new_connection( int );
	void		Wait_for_incoming_events( int );
	void		set_server_data(data_serv&);
	void		Destruct_client(  );
};

#endif /* SOCKET_HPP */