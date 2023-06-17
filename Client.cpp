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

#include "Client.hpp"

Client:: Client(int fd, string const &hostname, int port)
	: fd(fd), hostname(hostname), port(port), is_registered(NOT_REGISTER)
{
	username = "";
	nickname = "";
	buf = "";
	// channels.clear();
	status = 0;
}

Client::~Client(){}

//getter
const string &Client::getHostName(void) const {return hostname;}
const string &Client::getUserName(void) const {return username;}
const string &Client::getNickName(void) const {return nickname;}
const string &Client::getRealName(void) const {return realname;}
int Client::getFd(void) const {return fd;}
int Client::getPortNumber(void) const {return port;}
int Client::getRegistered(void) const {return is_registered;}

//setter
int Client::setNickName(string const &nick)
{
	if (nick.length() == 0)
		return (1);
	nickname = nick;
	return (0);
}

int Client::setUserName(string const &nick)
{
	if (nick.length() == 0)
		return (1);
	username = nick;
	return (0);
}

int Client::setRealName(string const &nick)
{
	if (nick.length() == 0)
		return (1);
	realname = nick;
	return (0);
}

void Client::setRegistered(void){is_registered = !is_registered;}

void Client::write(const string &msg) const
{
	string buffer = msg + "\r\n";
	cout << MAGENTA << fd << " >> " << msg << RESET << endl;
	if (send(fd, buffer.c_str(), buffer.length(), 0) < 0)
		throw runtime_error("Write Error");
}

// int Client::join(Channel *channel)
// {
// 	if (find(channels.begin(), channels.end(), channel) != channels.end())
// 		return (1);
// 	channels.push_back(channel);
// 	return (0);
// }

// int Client::kicked(Channel *channel)
// {
// 	channel_iterator it;
// 	if ((it = find(channels.begin(), channels.end(), channel)) == channels.end())
// 		return (1);
// 	channels.erase(it);
// 	return (0);
// }

/*void Client::clear()
{
 	for(channel_iterator it = channels.begin(); it != channels.end(); it++)
 		(it)->second->eraseClient(this);
}
*/
