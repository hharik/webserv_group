/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 14:45:56 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/01 01:10:43 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include "parsing.hpp"
#include "request.hpp"
#include "response.hpp"

struct data_header {
	std::string Content_type;
	std::string Host;
	int			Content_Length;
	std::string transfer_encoding;
	std::string method;
	std::string uri;
	std::string http_version;

	std::string boundary;
	int res_status;
	data_header() : Content_Length(-2), res_status(-2) {std::cout << "data_header ... created " << std::endl;}
	void print() {
		std::cout << " h" << Content_type <<  " " << Host << " " << Content_Length << " " << transfer_encoding << " " << method << "res : " << res_status << std::endl;
	}
	bool empty() {
		if (Host.empty() == true && transfer_encoding.empty() == true && Content_type.empty() == true)
			return true;
		return false;
	}
};

class Message
{
	const data_serv		*server_data;
	data_header			*header_data;
	request				client_request;
	response			client_response;
public:
	Message( const data_serv * );
	void	Request_message( std::string& );
	void	Respons_message( int );
	int		status_code();
	bool	eof();
};

#endif /* MESSAGE_HPP */