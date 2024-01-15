/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 13:47:45 by fhassoun          #+#    #+#             */
/*   Updated: 2024/01/12 13:25:54 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <cerrno>		// For errno

#include "Webserv.hpp"

int main(int argc, char **argv, char **env)
{
	// Server server;
	Webserv webserv;

	webserv.setEnv(env);
	(void)argv;
	if (argc == 1)
	{
		webserv.logging("No config file specified. Using default config!", INFO);
		// std::cout << "No config file specified. Using default config!" << std::endl;
		//server.parseConfig((char *)DEF_CONF);
		int parseReturn = webserv.parseConfig("configs/default.conf");
		if (parseReturn == 1) {
			std::cout << "Error: Unable to open the " << argv[1] << " file." << std::endl;
			return 1;
		} else if (parseReturn == 2) {
			std::cout << "Error: Invalid configuration." << std::endl;
			return 1;
		}
		std::vector<Config *> serverConfigs = webserv.getConfig();
		std::cout << "Number of server configurations: " << serverConfigs.size() << std::endl;
		Config::printConfigs(serverConfigs);
		webserv.init_servers();
		webserv.run();
	}
	else if (argc > 2)
	{
		std::cout << "Error : too many arguments." << std::endl;
		return (0);
	}
	else
	{
		int parseReturn = webserv.parseConfig(argv[1]);
		if (parseReturn == 1) {
			std::cout << "Error: Unable to open the " << argv[1] << " file." << std::endl;
			return 1;
		} else if (parseReturn == 2) {
			std::cout << "Error: Invalid configuration." << std::endl;
			return 1;
		}
		std::vector<Config *> serverConfigs = webserv.getConfig();
		std::cout << "Number of server configurations: " << serverConfigs.size() << std::endl;
		Config::printConfigs(serverConfigs);

		for (std::vector<Config *>::iterator itz = serverConfigs.begin(); itz != serverConfigs.end(); ++itz)
		{
			delete *itz;
  		}
		// webserv.init_servers();
		// webserv.run();
		// std::vector<Config>::iterator iter = server._config.begin();
		
		// webserv.init_servers();
	}
	
	
}