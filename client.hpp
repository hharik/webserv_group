/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:51:32 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/05 18:58:22 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "parsing.hpp"
#include "request.hpp"
#include "response.hpp"

struct data_header {
	/* add this iterator for a map loaction*/
	std::map<std::string, std::map<std::string, std::string> >::const_iterator it;
	std::string Content_type;
	std::string Host;
	int			Content_Length;
	std::string transfer_encoding;
	std::string method;
	std::string uri;
	std::string http_version;
	std::string boundary;
	/* add this two attribute to data_header */
	std::string	new_uri;
	std::string	query;
	std::string	redirect_path;
	std::string	methods;
	bool	is_redirect;
	std::string file;
	
	int 	res_status;
	/* the default value of res_status is updated if there is no problem  */
	data_header() : Content_Length(-2), res_status(0), is_redirect(false) { }
	void print() {
		std::cout << " h" << Content_type <<  " " << Host << " " << Content_Length << " " << transfer_encoding << " " << method << "res : " << res_status << std::endl;
	}
	bool empty() {
		if (Host.empty() == true && transfer_encoding.empty() == true && Content_type.empty() == true)
			return true;
		return false;
	}
};

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
	data_header			*header_data;
	request				client_request;
	response			client_response;
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