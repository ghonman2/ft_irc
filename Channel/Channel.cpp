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

#include "Channel.hpp"

Channel::Channel(string const &name) : name(name)
{
	clients.clear();
}

Channel::~Channel(){}

static string USER_ID(string nick, string user, string host)
{
    return nick+"!"+user+"@"+host;
}

void Channel::setOperator(Client *client)
{
	if (clients.find(client) == clients.end())
		return ;
	clients[client] = OPER; 
}

void Channel::setNotOperator(Client *client)
{
	if (clients.find(client) == clients.end())
		return ;
	clients[client] = NON_OPER; 
}

Client * Channel::getClientByName(const string &name)
{
	for(client_iterator it = clients.begin(); it != clients.end(); it++)
		if (it->first->getNickName() == name)
			return (it->first);
	return (NULL);
}

int Channel::addClient(Client *client)
{
	if (clients.size() >= MAX_CLIENT_IN_CHANNEL || clients.find(client) != clients.end())
		return (1);
	clients.insert(make_pair(client, NON_OPER));
	string user_id=USER_ID(client->getNickName(), client->getUserName(), client->getHostName());
	for(client_iterator it = clients.begin(); it != clients.end(); it++)
		(it)->first->write(":"+user_id+" JOIN :"+this->getName());
	cout << user_id << " joined the channel " << this->getName() << endl;
	return (0);
}

int Channel::isOperator(Client *client)
{
	return clients[client];
}

int Channel::isClient(Client *client)
{
	if (clients.find(client) == clients.end())
		return (0);
	return (1);
}

const string &Channel::getName(void) const{return (name);}

void Channel::eraseClient(Client *client)
{
	clients.erase(client);
}

void Channel::privmsg(const Client *client, const string &msg)
{
	string user_id=USER_ID(client->getNickName(), client->getUserName(), client->getHostName());
	for(client_iterator it = clients.begin(); it != clients.end(); it++)
		if((it)->first->getNickName() != client->getNickName())
			(it)->first->write(":"+user_id+" PRIVMSG "+this->getName()+" :"+msg);
}

void Channel::broadcast(const string &msg)
{
	for(client_iterator it = clients.begin(); it != clients.end(); it++)
		(it)->first->write(msg);
}

void Channel::part(Client *client)
{
	string user_id=USER_ID(client->getNickName(), client->getUserName(), client->getHostName());
	for(client_iterator it = clients.begin(); it != clients.end(); it++)
		(it)->first->write(":"+user_id+" PART :"+this->getName());
	eraseClient(client);
}

void Channel::quit(Client *client, string msg)
{
	string user_id=USER_ID(client->getNickName(), client->getUserName(), client->getHostName());
	broadcast(":"+user_id+" QUIT :Quit: "+msg);
	eraseClient(client);
}

map<Client*, int> &Channel::getClients(void) {return (clients);}
