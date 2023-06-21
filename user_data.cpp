/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user_data.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 15:44:21 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/21 16:06:06 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "user_data.hpp"
#include "client.hpp"

User_data::User_data()
{
	_is_server = false;
	_is_terminated = false;

}

User_data::~User_data()
{
	delete client;
}

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

void	User_data::set_is_terminated( bool status )
{
	_is_terminated = status;	
}

bool	User_data::get_is_terminated( ) const
{
	return (_is_terminated);
}
