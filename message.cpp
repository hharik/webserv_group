/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   message.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 14:48:59 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/01 13:38:46 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "message.hpp"
#include "parsing.hpp"

Message::Message( const data_serv *dptr ):server_data(dptr), header_data(new data_header()), 
										client_request(dptr, header_data), client_response(dptr, header_data)
{

}

void	Message::Request_message( std::string &buffer )
{
	client_request.parse(buffer);
}

void	Message::Respons_message( int fd, std::string &buffer )
{
	client_response.find_required_location();
}

bool	Message::eoh()
{
	return client_request.end_of_header;
}

int	Message::status_code()
{
	return header_data->res_status;
}
