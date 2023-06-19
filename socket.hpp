/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:50:54 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/19 11:00:02 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "parsing.hpp"

class Socket
{
	/* server attribute */
	data_serv				*server_data;
	User_data				*user_data;
	bool					_ierver;
	int						sockfd;
	struct addrinfo			hints;
	struct addrinfo			*result;
	int						reuseaddr_opt;
	int 					status;
	int						events_nbr;
	struct	kevent			server_event;
	std::vector<Client*> 	clients;
	bool					new_client;
	Client					*client_ptr;
	int						clients_ind;
public:
	Socket();
	~Socket();
	Socket(const std::string&, const std::string&);
	
	/* member functions */
	int			Create_the_socket();
	void		attach_server_socket( int );
	void		multiplexing( int );
	void		Accept_new_connection( int );
	void		Wait_for_incoming_events( int );
	void		set_server_data(data_serv&);
	void		Destruct_client( Client * );
};

#endif /* SOCKET_HPP */