/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 14:45:56 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/04 00:02:54 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

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

class Message
{
	const data_serv		*server_data;
	data_header			*header_data;
	request				client_request;
	response			client_response;
public:
	Message( const data_serv * );
	void	Request_message( std::string& );
	void	Respons_message( int, std::string& );
	int		status_code();
	bool	eoh();
	bool	eof();
};

#endif /* MESSAGE_HPP */