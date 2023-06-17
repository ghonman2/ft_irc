/**********************************************************************
 * Copyright (c) 2023
 *  Joowon park <ghonman2@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/
#ifndef CONNECTION_HPP
#define CONNECTION_HPP 

class Server;

#include <poll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include "types.h"
#include "Channel.hpp"
#include "Client.hpp"
#include "headers.h"
#include "CommandManager.hpp"
#include "Bot.hpp"

#define CLIENT_MAX OPEN_MAX
#define MAX_CONNECTIONS ( CLIENT_MAX + 1 )
#define PAGE_SIZE (1UL << 12)
#define MSG_SIZE PAGE_SIZE

using namespace std;

enum ServerType
{
	READY,
	RUNNING,
};

class Bot;
class Server
{
	typedef vector<struct pollfd>::iterator poll_iterator;
	typedef map<int, Client *>::iterator client_iterator;
	typedef map<string, Channel *>::iterator channel_iterator;
	map<string, Channel*> channels;
	map<int, Client*> clients;

	private :
		Bot *bot;

		int is_running; 
		int	socket_fd;
		struct sockaddr_in server_addr;
		vector<struct pollfd> p_fds;
		int port_num;
		
		CommandManager *cmds;

		void onConnection(void);
		int getMessage(int fd);

	public : 
		Client* find_by_nickname(string nick);	
		string passwd;
		
		int init(char *av[]);
		Server();
		~Server();
		int newSocket(void);
		int start(void);
		void disConnection(int fd);

		const string &getPasswd(void) const;
		map<string, Channel*> &getChannels(void);
		map<int, Client *> &getClients(void);
		Client* getClientByName(const string name);
		Bot* getBot();
};

#endif /* ifndef CONNECTION_HPP */
