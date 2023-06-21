/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ajemraou <ajemraou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/25 14:52:53 by ajemraou          #+#    #+#             */
/*   Updated: 2023/06/20 20:10:30 by ajemraou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"

std::map<std::string, std::string> parsing::mime_type;
std::map<std::string, std::string> parsing::mime_types_ay;

// void signalHandler(int signal)
// {
// 	std::cout << "broken po " << signal << std::endl;
// }

void sigpipe_handler(int signal) {
    // Handle or ignore the SIGPIPE signal here
    // In this example, we simply print a message
    printf("Received SIGPIPE signal, ignoring... %d\n", signal);
}

int main(int ac, char **av)
{

    // signal(SIGINT, signalHandler);
	signal(SIGPIPE, SIG_IGN);
	if (ac == 2)
	{
		Server server(av[1]);
		server.Create_http_servers();
	}
}