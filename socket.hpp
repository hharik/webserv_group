/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:50:54 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/20 20:24:02 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKET_HPP
#define SOCKET_HPP

#include "parsing.hpp"
#include <_types/_intmax_t.h>

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
	unsigned long	long	index;
	struct				sockaddr client;
	socklen_t			len;
public:
	Socket();
	~Socket();
	Socket(const std::string&, const std::string&);
	
	/* member functions */
	int			Create_the_socket();
	void		attach_server_socket( int );
	void		multiplexing( int );
	int			Accept_new_connection( int );
	void		Wait_for_incoming_events( int );
	void		set_server_data(data_serv&);
	void		Destruct_client( Client * );
	int			client_connection( );
};

#endif /* SOCKET_HPP */