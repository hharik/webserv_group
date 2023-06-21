/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:52:53 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/21 16:57:17 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

std::map<std::string, std::string> parsing::mime_type;
std::map<std::string, std::string> parsing::mime_types_ay;

int main(int ac, char **av)
{
	std::string Default_config = "./Default.conf";
	signal(SIGPIPE, SIG_IGN);

	if (ac == 2)
	{
		Server server(av[1]);
		server.Create_http_servers();
	}
	else 
	{
		Server server(Default_config);
		server.Create_http_servers();
	}
}