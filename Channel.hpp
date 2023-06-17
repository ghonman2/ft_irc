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

#ifndef CHANNEL_HPP
#define CHANNEL_HPP 

class Channel;

#include <poll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include "headers.h"
#include "types.h"
#include "Server.hpp"
#include "Client.hpp"

#define MAX_CLIENT_IN_CHANNEL 3

using namespace std;

enum ChannelType
{
	NON_OPER,
	OPER,
};

class Channel
{
	typedef map<Client*, int>::iterator client_iterator;
	//typedef vector<Client*>::const_iterator const_client_iterator;
	//typedef vector<Channel*>::iterator channel_iterator;
	private :
		const string name;
		map<Client*, int> clients;

	public :
		explicit Channel(string const &name);
		
		~Channel();
		
		int addClient(Client *client);
		void eraseClient(Client* client);
		const string &getName(void) const;
		void privmsg(const Client *client, const string &msg);
		void broadcast(const string &msg);
		void part(Client *client);
		void quit(Client *client, string msg);
		void setOperator(Client *client);
		void setNotOperator(Client *client);
		int isOperator(Client *client);
		int isClient(Client *client);
		map<Client*, int>&getClients(void);
		Client * getClientByName(const string &name);
};

#endif /* ifndef CHANNEL_HPP */
