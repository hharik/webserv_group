/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user_data.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 15:43:32 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/19 10:48:05 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_DATA_HPP
#define USER_DATA_HPP

#include "parsing.hpp"

class User_data
{
	bool	_is_server;
	bool	_is_terminated;
	Socket	*socket;
	Client	*client;
public:
	User_data();
	~User_data();

	void	set_status( bool );
	void	set_is_terminated( bool );
	void	set_socket( Socket* );
	void	set_client( Client* );	
	
	bool 	get_status() const;
	bool	get_is_terminated( ) const;
	Socket	*get_socket() const;
	Client	*get_client() const;	
};

#endif /* USER_DATA_HPP */