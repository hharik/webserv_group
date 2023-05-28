/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   user_data.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/28 15:43:32 by ajemraou          #+#    #+#             */
/*   Updated: 2023/05/28 15:47:35 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_DATA_HPP
#define USER_DATA_HPP

class Client;
class Socket;

class User_data
{
	bool	_is_server;
	Socket	*socket;
	Client	*client;
public:
	void	set_status( bool );
	void	set_socket( Socket* );
	void	set_client( Client* );	
	
	bool get_status() const;
	Socket	*get_socket() const;
	Client	*get_client() const;	
};

#endif /* USER_DATA_HPP */