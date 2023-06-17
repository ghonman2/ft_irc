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

#ifndef CLIENT_HPP
#define CLIENT_HPP 

class Client;

#include <poll.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <set>
#include "Server.hpp"
#include "Channel.hpp"
#include "headers.h"
#include "types.h"

using namespace std;

enum ClientType
{
	NOT_REGISTER,
	REGISTER,
	REG_CAP,
	REG_PASS
};

class Client
{
	typedef set<string>::iterator channel_iterator;

	private :
		const int fd;
		const string hostname;
		const int port;
		string nickname;
		string username;
		string realname;
		int is_registered;

	public :
		set<string> channels;

		int status;
		string buf;
		explicit Client(int fd, string const &hostname, int port);	
		~Client();
		//getter 
		const string &getHostName(void) const;
		const string &getNickName(void) const;
		const string &getUserName(void) const;
		const string &getRealName(void) const;
		int getFd(void) const;
		int getPortNumber(void) const;
		int getRegistered(void) const;

		// setter
		int setNickName(string const &nick);
		int setUserName(string const &nick);
		int setRealName(string const &nick);
		void setRegistered(void);

		//Command Function
		void write(const string &msg) const;
		// int join(Channel *channel);
		// int kicked(Channel *channel);
		
		void clear();
};

#endif /* ifndef CLIENT_HPP */
