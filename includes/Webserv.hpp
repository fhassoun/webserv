/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fhassoun <fhassoun@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/20 08:10:32 by fhassoun          #+#    #+#             */
/*   Updated: 2023/11/22 12:10:55 by fhassoun         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <cstdlib>		// For exit() and EXIT_FAILURE
#include <iostream>		// For cout
#include <unistd.h>		// For read
#include <cerrno>		// For errno
#include <vector>
#include <map>
#include <iomanip>
#include <cctype>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <typeinfo>
#include <cmath>
#include <string>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <fcntl.h>

#include "ASocket.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "helpers.hpp"

#define DEF_CONF		"configs/default.conf"
#define DEF_PORT		9999
#define DEF_BACKLOG		100
#define FALSE			0
#define TRUE			1


// define log levels
#define DEBUG			2
#define INFO			3
#define WARNING			4
#define ERROR			5

// define colors
#define RESET			"\033[0m"
#define BLACK			"\033[30m"				/* Black */
#define RED				"\033[31m"				/* Red */
#define GREEN			"\033[32m"				/* Green */
#define YELLOW			"\033[33m"				/* Yellow */
#define BLUE			"\033[34m"				/* Blue */
#define MAGENTA			"\033[35m"				/* Magenta */
#define CYAN			"\033[0;38;5;44m"		/* Cyan */
#define WHITE			"\033[37m"				/* White */
#define LILA            "\033[0;38;5;199m"		/* Lila */
#define PURPLE			"\033[38;5;105m"		/* Purple */
#define ORANGE			"\033[38;5;208m"		/* Orange */



class Config;
class Client;
class Server;

struct HttpRequest {
    std::string http_version;
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
};

struct HttpResponse {
    std::string http_version;
    int status_code;
    std::string status_message;
    std::map<std::string, std::string> headers;
    std::string body;
};

class Webserv
{
	private:
		int		sockfd;
		int		new_sd ;
		char	buffer[80];
		int		rc;
		int 	end_server ;
		int		close_conn ;
		HttpRequest http_request;
		HttpResponse http_response;
		std::vector<Server> server;
		std::vector<Server>::iterator s_iter;
		
		std::vector<Client> client;
		std::vector<Server>::iterator cl_iter;

		std::vector<Config> config;
		std::vector<Server>::iterator co_iter;

		std::vector<pollfd> poll_fd;
		std::vector<pollfd>::iterator p_iter;
		
		std::map <int, std::string> in_request;
		std::map <int, std::string> out_response;

	
	public:
		std::string response;
		Webserv();
		~Webserv();
		Webserv(Webserv const &src);
		Webserv &operator=(Webserv const &src);
		
		//setters
		void setSockfd(int sockfd);
		void setServer(std::vector<Server> server);
		void setClient(std::vector<Client> client);
		void setConfig(std::vector<Config> config);
		void setInRequest(std::map <int, std::string> in_request);
		void setOutResponse(std::map <int, std::string> out_response);


		//getters
		int getSockfd();
		std::vector<Server> getServer();
		std::vector<Client> getClient();
		std::vector<Config> getConfig();
		std::map <int, std::string> getInRequest() ;
		std::map <int, std::string> getOutResponse();
	
		
		//methods
		
		void parseConfig(char *path);
		void init_servers();
		void run();
		void logging(std::string str, int status);
		int handle_pollin(int i);
		HttpRequest parse_http_request(const std::string& request);
		std::string create_http_response(void);
	
};