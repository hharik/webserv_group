/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:51:32 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/02 20:24:42 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "parsing.hpp"
#include "message.hpp"
#include <sys/_types/_intptr_t.h>


class Client
{
	const data_serv		*server_data;
	User_data			*user_data;
	int					fd;
	char				buffer[BUFFER_SIZE];
	struct				kevent	client_event[2];
	struct				sockaddr client;
	socklen_t			len;
	int					nbytes;
	std::string			request_buffer;
	Message				client_message;
public:
	Client( const data_serv* );
	// bool			is_reading;
	// void	set_server_data( data_serv* );
	void	client_connection( int );
	void	attach_client_socket( int );
	void	read_from_socket();
	void	send_the_response();

	bool eof();
	int	get_fd();
};

#endif /* CLIENT_HPP */