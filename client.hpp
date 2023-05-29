/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:51:32 by ajemraou          #+#    #+#             */
/*   Updated: 2023/05/29 13:58:49 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "request.hpp"
#include "response.hpp"
#include "parsing.hpp"

class Client
{
	User_data		*user_data;
	int				fd;
	char	buffer[BUFFER_SIZE];	
	struct	kevent	client_event[2];
	struct	sockaddr client;
	socklen_t	len;
	int				nbytes;
	std::string		request_buffer;
	response		client_response;
	request			client_request;
public:
	Client();
	// bool			is_reading;
	void	client_connection( int );
	void	attach_client_socket( int );
	void	read_from_socket();

};

#endif /* CLIENT_HPP */