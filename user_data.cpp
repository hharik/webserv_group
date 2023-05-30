/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user_data.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 15:44:21 by ajemraou          #+#    #+#             */
/*   Updated: 2023/05/28 15:46:16 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "user_data.hpp"

void	User_data::set_status( bool status )
{
	_is_server = status;
}

void	User_data::set_socket( Socket* sck)
{
	socket = sck;
}

void	User_data::set_client( Client* cli)
{
	client = cli;	
}

bool 	User_data::get_status() const
{
	return _is_server;
}

Socket	*User_data::get_socket() const
{
	return socket;
}

Client	*User_data::get_client() const
{
	return client;
}