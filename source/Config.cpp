/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sung-hle <sung-hle@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/13 11:44:28 by fhassoun          #+#    #+#             */
/*   Updated: 2024/01/12 11:16:45 by sung-hle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config()	: listen(""), root(""), autoindex(false), port(0), client_body_buffer_size(0){
}

Config::~Config()	{
	for (std::map<std::string, Location*>::iterator it = location.begin(); it != location.end(); ++it) {
		delete it->second;
	}
}

Config::Config(Config const &src)	: listen(src.listen), root(src.root), autoindex(src.autoindex), port(src.port), client_body_buffer_size(src.client_body_buffer_size){
	*this = src;
}

Config &Config::operator=(Config const &src)
{
	if (this == &src)	{
		listen = src.listen;
		serverNames = src.serverNames;
		host = src.host;
		root = src.root;
		errorPage = src.errorPage;
		location = src.location;
		allowedMethods = src.allowedMethods;
		index = src.index;
		autoindex = src.autoindex;
		port = src.port;
		client_body_buffer_size = src.client_body_buffer_size;

		// Delete existing Location objects
		for (std::map<std::string, Location*>::iterator it = location.begin(); it != location.end(); ++it) {
			delete it->second;
		}

		// Clear the map
		location.clear();

		// Copy Location objects
		for (std::map<std::string, Location*>::const_iterator it = src.location.begin(); it != src.location.end(); ++it) {
			Location *newLocation = new Location(*(it->second));
			location[it->first] = newLocation;
		}
	}
	return (*this);
}


int Config::parse(std::ifstream& configFile) {
	std::string line;
	std::string tmp;
	std::string tmp2;
	// std::cout << "Parsing config file..." << std::endl;
	while (std::getline(configFile, line)) {
		std::istringstream iss(line);
		std::string keyword;
		iss >> std::ws >> keyword;

		if (keyword == "server") {
			// Start parsing the server block
			size_t openingBracePos = line.find("{");
			if (openingBracePos != std::string::npos) {
				// Opening curly bracket found on the same line
				line.erase(0, openingBracePos + 1);  // Erase everything before {
			} else {
				// Look for the opening curly bracket in the following lines
				while (std::getline(configFile, line)) {
						size_t openingBracePos = line.find("{");
						if (openingBracePos != std::string::npos) {
								// Opening curly bracket found on this line
								line.erase(0, openingBracePos + 1);  // Erase everything before {
								break;
						}
				}
			}
			while (std::getline(configFile, line)) {
				if (line.find("}") != std::string::npos) {
					return 0;
//---------------------------                    
				} else if (line.find("listen") != std::string::npos) {
					keyword = "listen";
					iss.clear();
					iss.str(line);
					iss >> keyword >> tmp;
					setListen(tmp);
					//port, host, server_name
				}
				// Parse other server-related configuration here
				else if (line.find("server_name") != std::string::npos) {
					keyword = "server_name";
					iss.clear();
					iss.str(line);
					iss >> keyword;
					while (iss >> tmp) {
						formatString(tmp);
						setServerNames(tmp);
					}
				} else if (line.find("host") != std::string::npos) {
					keyword = "host";
					iss.clear();
					iss.str(line);
					iss >> keyword >> tmp;
					// std::cout << "TMP HOST: " << tmp << std::endl;
					formatString(tmp);
					host.push_back(tmp);
				} else if (line.find("root") != std::string::npos) {
					keyword = "root";
					iss.clear();
					iss.str(line);
					iss >> keyword >> tmp;
					formatString(tmp);
					setRoot(tmp);
				} else if (line.find("error_page") != std::string::npos) {
					keyword = "error_page";
					iss.clear();
					iss.str(line);
					std::string tmp2;
					iss >> keyword >> tmp >> tmp2;
					formatString(tmp2);
					setErrorPage(tmp, tmp2);
				} else if (line.find("client_max_body_size") != std::string::npos) {
					keyword = "client_max_body_size";
					iss.clear();
					iss.str(line);
					iss >> keyword >> tmp;
					formatString(tmp);
					setClientBodyBufferSize(tmp);
				} else if (line.find("location") != std::string::npos) {
					std::istringstream iss(line);
					iss >> keyword;
					if (keyword == "location") {
						size_t openingBracePos = line.find("{");
						if (openingBracePos != std::string::npos) {
							tmp = line.substr(line.find("location") + 8, openingBracePos - line.find("location") - 8);
							std::istringstream issTmp(tmp);
							issTmp >> std::ws;
							std::getline(issTmp, tmp2, ' ');
							line.erase(0, openingBracePos + 1);  // Erase everything before {
						} else {
							while (std::getline(configFile, line)) {
								size_t openingBracePos = line.find("{");
								if (openingBracePos != std::string::npos) {
									tmp = line.substr(line.find("location") + 8, openingBracePos - line.find("location") - 8);
									std::istringstream issTmp(tmp);
									issTmp >> std::ws;
									std::getline(issTmp, tmp2, ' ');
									line.erase(0, openingBracePos + 1);  // Erase everything before {
									break;
								}
							}
						}
					}
					setLocation(tmp2, configFile);
				} else if (line.find("autoindex") != std::string::npos) {
					keyword = "autoindex";
					iss.clear();
					iss.str(line);
					iss >> keyword >> tmp;
					formatString(tmp);
					if (tmp.compare("on") == 0)
						this->autoindex = true;
					else
						this->autoindex = false;
				} else if (line.find("index") != std::string::npos) {
					keyword = "index";
					iss.clear();
					iss.str(line);
					iss >> keyword;
					while (iss >> tmp) {
						formatString(tmp);
						setIndex(tmp);
					}
				}
			}
			// End of server block
		} else
			return 1;
	}
	return 2;
}


void Config::setListen(std::string str) {
	listen = str;
}

const std::string& Config::getListen()  const {
	return listen;
}

void Config::setServerNames(std::string str) {
	serverNames.push_back(str);
}

const std::vector<std::string>& Config::getServerNames() const
{
	return serverNames;
}

void Config::setHost(std::string str)
{
	this->host.push_back(str);
}

const std::vector<std::string>& Config::getHost() const
{
	return (this->host);
}

void Config::setPort(std::string str) {
	int _port = 0;
	_port = static_cast<int>(std::strtol(str.c_str(), NULL, 10));
	port = _port;
}

int Config::getPorts() const {
	return (this->port);
}

void Config::setRoot(std::string str)
{
	this->root = str;
}

const std::string& Config::getRoot() const
{
	return (this->root);
}

void Config::setErrorPage(std::string str, std::string str2)
{
	int code = 0;
			code = static_cast<int>(std::strtol(str.c_str(), NULL, 10));
			this->errorPage.insert(std::pair<int, std::string>(code, str2));
}

const std::map<int, std::string>& Config::getErrorPage() const
{
	return (this->errorPage);
}

void Config::setClientBodyBufferSize(std::string str) {
	unsigned long _client_body_buffer_size = 0;
	_client_body_buffer_size = static_cast<unsigned long>(std::strtol(str.c_str(), NULL, 10));
	client_body_buffer_size = _client_body_buffer_size;
}

unsigned long Config::getClientBodyBufferSize() const {
	return (this->client_body_buffer_size);
}

void Config::setLocation(std::string str, std::ifstream& configFile) {
	std::string line;
	std::string tmp;
	size_t pos;
	Location *loc = new Location();

	while (std::getline(configFile, line)) {
		std::istringstream iss(line);
		std::string keyword;
		iss >> keyword;

		loc->setPath(str);

		if (line.find("}") != std::string::npos) {
			// Closing curly bracket found, end parsing server block
			// std::cout << "END OF LOCATION" << std::endl;
			location.insert(std::make_pair(str, loc));
			break;
		} else if ((pos = line.find("allow_methods")) != std::string::npos) {
			std::istringstream iss(line.substr(pos + strlen("allow_methods") + 1));
			std::set<std::string> methods;
			std::string method;
			while (iss >> method) {
				formatString(method);
				methods.insert(method);
			}
			// for (std::set<std::string>::const_iterator it = methods.begin(); it != methods.end(); ++it) {
			// 		std::cout << *it << " ";
			// }
			std::cout << std::endl;
			loc->setAllowedMethods(methods);
		} else if (line.find("root") != std::string::npos) {
			keyword = "root";
			iss.clear();
			iss.str(line);
			iss >> keyword >> tmp;
			formatString(tmp);
			loc->setRoot(tmp);
		} else if (line.find("autoindex") != std::string::npos) {
			keyword = "autoindex";
			iss.clear();
			iss.str(line);
			iss >> keyword >> tmp;
			formatString(tmp); 	
			if (tmp.compare("on") == 0)
				loc->setAutoindex(true);
			else
				loc->setAutoindex(false);
		} else if (line.find("index") != std::string::npos) {
			keyword = "index";
			iss.clear();
			iss.str(line);
			iss >> keyword;
			while (iss >> tmp) {
				formatString(tmp);
				loc->setIndex(tmp);
			}
		} else if ((pos = line.find("cgi_path")) != std::string::npos) {
			std::istringstream iss(line.substr(pos + strlen("cgi_path") + 1));
			std::vector<std::string> cgiPathMap;
			while (iss >> tmp) {
				formatString(tmp);
				cgiPathMap.push_back(tmp);
			}
			loc->setCGIPath(cgiPathMap);
		} else if ((pos = line.find("cgi_ext")) != std::string::npos) {
			std::istringstream iss(line.substr(pos + strlen("cgi_ext") + 1));
			std::vector<std::string> cgiExtVector;
			while (iss >> tmp) {
				formatString(tmp);
				cgiExtVector.push_back(tmp);
			}
			loc->setCGIExt(cgiExtVector);
		}
	}
}

const std::map<std::string, Location*>& Config::getLocation() const
{
	return (this->location);
}

void Config::setAllowedMethods(std::string str)
{
	std::string tmp;
	std::string tmp2;

	std::stringstream ss(str);
	while (std::getline(ss, tmp, ' '))
	{
		if (tmp.compare("allow_methods") == 0)
		{
			std::getline(ss, tmp, ' ');
			tmp.erase(tmp.size() - 1);
			std::getline(ss, tmp2, ' ');
			tmp2.erase(tmp2.size() - 1);
			this->allowedMethods.insert(tmp2);
		}
	}
}

const std::set<std::string>& Config::getAllowedMethods() const
{
	return (this->allowedMethods);
}

void Config::setIndex(std::string str)
{
	index.push_back(str);
}

const std::vector<std::string>& Config::getIndex() const
{
	return index;
}

void Config::setAutoindex(bool b)
{
	autoindex = b;
}

bool Config::getAutoindex() const
{
	return (autoindex);
}

void displaySet(const std::set<std::string>& strSet) {
    for (std::set<std::string>::const_iterator it = strSet.begin(); it != strSet.end(); ++it) {
        std::cout << "." << *it << ".";
    }
}

// Helper function to display a vector of strings
void displayVector(const std::vector<std::string>& strVector) {
    for (std::vector<std::string>::const_iterator it = strVector.begin(); it != strVector.end(); ++it) {
        std::cout << "." << *it << ".";
    }
}

void Config::printConfigs(std::vector<Config *>& serverConfigs) {
	for (std::vector<Config *>::iterator itz = serverConfigs.begin(); itz != serverConfigs.end(); itz++) {
    std::cout << "Server Configuration:" << std::endl;
    std::cout << "Listen: ." << (*itz)->getListen() << "." << std::endl;
    // std::cout << "Server Name: ." << (*itz)->getServerNames() << "." << std::endl;
    std::cout << "Server Name: .";
		const std::vector<std::string>& serverNames = (*itz)->getServerNames();
		displayVector(serverNames);
		std::cout << "." << std::endl;
		
    const std::vector<std::string>& hosts = (*itz)->getHost();
    std::cout << "Hosts: ";
    displayVector(hosts);
    std::cout << std::endl;

    std::cout << "Root: ." << (*itz)->getRoot() << "." << std::endl;
		std::cout << "Index: .";
		const std::vector<std::string>& index = (*itz)->getIndex();
		displayVector(index);
		std::cout << "." << std::endl;
		std::cout << "Autoindex: ." << ((*itz)->getAutoindex() ? "on" : "off") << std::endl;
    const std::map<int, std::string>& errorPages = (*itz)->getErrorPage();
    std::cout << "Error Pages:" << std::endl;
    for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
        std::cout << "\t." << it->first << ". => ." << it->second << "." << std::endl;
    }
		std::cout << "Client Body Buffer Size: ." << (*itz)->getClientBodyBufferSize() << "." << std::endl;

//Locations
		const std::map<std::string, Location*>& locations = (*itz)->getLocation();
		std::cout << "Locations:" << std::endl;

		for (std::map<std::string, Location*>::const_iterator it = locations.begin(); it != locations.end(); ++it) {
			std::cout << "Path: ." << it->first << "." << std::endl;
			std::cout << "\tRoot: ." << it->second->getRoot() << "." << std::endl;
			std::cout << "\tAutoindex: ." << (it->second->getAutoindex() ? "on" : "off") << std::endl;
			std::cout << "\tAllowed Methods: .";
			displaySet(it->second->getAllowedMethods());
			std::cout << std::endl;
			std::cout << "\tIndex: ." << it->second->getIndex() << "." << std::endl;
			std::cout << "\tcgi_path: ." << std::endl;
			const std::vector<std::string>& cgiPath = it->second->getCGIPath();
			for (std::vector<std::string>::const_iterator pathIt = cgiPath.begin(); pathIt != cgiPath.end(); ++pathIt) {
				std::cout << "\t\t." << *pathIt << "." << std::endl;
			}
			// Display cgi_ext
			std::cout << "\tcgi_ext: " << std::endl;
			const std::vector<std::string>& cgiExt = it->second->getCGIExt();
			for (std::vector<std::string>::const_iterator extIt = cgiExt.begin(); extIt != cgiExt.end(); ++extIt) {
				std::cout << "\t\t." << *extIt << "." << std::endl;
			}
		}
	}
}

void Config::formatString(std::string& str) {
	if (!str.empty()) {
		if (str[str.size() - 1] == ';')
			str.erase(str.size() - 1);
	}
}