/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:51:32 by ajemraou          #+#    #+#             */
/*   Updated: 2023/05/28 16:51:13 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP


#include "server.hpp"
#include "user_data.hpp"
#include "response.hpp"

// #include <iostream>
// #include <sys/_types/_socklen_t.h>
// #include <sys/types.h>
// #include <sys/event.h>
// #include <sys/time.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netdb.h>
// #include <fcntl.h>
// #include <vector>
// #include <unistd.h>
class response;

class Client
{
	User_data		*user_data;
	int				fd;
	char	buffer[BUFFER_SIZE];	
	struct	kevent	client_event[2];
	struct	sockaddr client;
	socklen_t	len;
	int				nbytes;
	std::string		client_request;
	response		client_response;
public:
	Client();
	// bool			is_reading;
	void	client_connection( int );
	void	attach_client_socket( int );
	void	read_from_socket();

};

#endif /* CLIENT_HPP */