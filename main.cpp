/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:52:53 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/20 09:07:17 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

std::map<std::string, std::string> parsing::mime_type;
std::map<std::string, std::string> parsing::mime_types_ay;

// void signalHandler(int signal)
// {
// 	std::cout << "broken po " << signal << std::endl;
// }

int main(int ac, char **av)
{

	{
    // signal(SIGINT, signalHandler);
	if (ac == 2)
	{
		Server server(av[1]);
		server.Create_http_servers();
	}
	}
}