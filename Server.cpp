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

#include "Server.hpp"

Server::Server(){
	cmds = new CommandManager(this);
	bot = new Bot();
};

Server::~Server()
{
	delete cmds;
	delete bot;

	for (poll_iterator it = p_fds.begin(); it != p_fds.end(); it ++)
	{
		if (it->fd > 0)
			close(it->fd);
		it->fd = 0;
	}
	if (socket_fd > 0)
		close(socket_fd);
};

static int isAllDigit(char *passwd)
{
	char *line = passwd;

	for (; *line ; line ++)
		if (!isdigit(*line))
			return (0);
	return (line != passwd);
}

static void printLogo(void)
{
	ifstream fin;
	string line;

	fin.open("./logo");
	if (!fin.is_open())
		perror("Error logo"), exit(1);
	while(getline(fin, line))
		cout << line << endl;
	fin.close();
}

int Server::init(char *av[])
{
	socket_fd = 0;
	is_running = READY;
	long long num;

	if (!strlen(av[2]) || !isAllDigit(av[2]))
		perror("Invalid password"), exit(1);
	if (!strlen(av[1]) || !isAllDigit(av[1]))
		perror("Invalid port number"), exit(1);
	num = atoi(av[1]);
	if (1024 > num || num > 49151)
		perror("Invalid port number"),exit(1);
	port_num = htons(atoi(av[1]));
	passwd = string(av[2]);
	return (0);
}

int Server::newSocket(void)
{
	struct pollfd poll_fd;

	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
		return (-1);
	bzero(&server_addr, sizeof(server_addr));
	server_addr = (struct sockaddr_in){
		.sin_family = AF_INET,
			.sin_addr.s_addr = htonl(INADDR_ANY),
			.sin_port = port_num,
	};
	if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1) 
		throw runtime_error("Error socket file control NON_BLOCKING");
	if(::bind(socket_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
		throw runtime_error("Bind error");
	if(listen(socket_fd, MAX_CONNECTIONS) == -1)
		throw runtime_error("Listen error");
	poll_fd = (struct pollfd){socket_fd, POLLIN, 0};
	p_fds.push_back(poll_fd);
	return (socket_fd);
}

int Server::start(void) 
{
	printLogo();
	is_running = RUNNING;
	while (is_running)
	{
		if (poll(p_fds.begin().base(), p_fds.size(), -1) < 0)
			throw runtime_error("Polling fd has an error");
		for(poll_iterator it = p_fds.begin(); it != p_fds.end(); it++)
		{
			if (it->revents == 0)
				continue;
			if ((it->revents & POLLHUP) == POLLHUP)
			{
				disConnection(it->fd);
				break ;
			}
			if ((it->revents & POLLIN) == POLLIN)
			{
				if (it->fd == socket_fd)
				{
					onConnection();
					break ;
				}
				if (getMessage(it->fd))
				{
					disConnection(it->fd);
					break;
				}
			}
		}
	}
	return (0);
}

void Server::onConnection(void)
{
	int fd;
	struct sockaddr_in address;
	socklen_t s_size = sizeof(address);
	char hostname[NI_MAXHOST];
	struct pollfd poll_fd;

	if (p_fds.size() >= MAX_CONNECTIONS)
		throw runtime_error("OverConnection"); // 이야기 해야할 것
	if ((fd = accept(socket_fd, (struct sockaddr *) &address, &s_size)) == -1)
		throw runtime_error("Accept Error");
	poll_fd = (struct pollfd){fd, POLLIN, 0};
	p_fds.push_back(poll_fd);
	if (getnameinfo((struct sockaddr *) &address, sizeof(address), 
				hostname, NI_MAXHOST, NULL, 0, NI_NUMERICSERV) != 0)
		throw std::runtime_error("Getnameinfo error");
	Client *client = new Client(fd, hostname, htons(address.sin_port));
	cout << GREEN << hostname << " " << client->getPortNumber() << " is Connected" << RESET << endl;
	clients.insert(make_pair(fd,client));
}

void Server::disConnection(int fd)
{
	try{
		Client *client = clients.at(fd);
		clients.erase(fd);
		for (poll_iterator it = p_fds.begin(); it != p_fds.end(); it ++)
		{
			if (it->fd != fd)
				continue;
			cout << RED << client->getHostName() << " " << client->getPortNumber() << " is disConnected" << RESET << endl;
			p_fds.erase(it);
			close(fd);
			break ;
		}
		for (set<string>::iterator it =  client->channels.begin(); it != client->channels.end(); it ++)
		{
			//Client *tmp = channels[*it]->getClientByName(client->getNickName());
			//if (tmp)
				channels[*it]->eraseClient(client);
		}
		delete client;
	}
	catch (const std::out_of_range &ex)
	{}
}

int Server::getMessage(int fd)
{
	client_iterator cl = clients.find(fd);
	char msg[MSG_SIZE];
	ssize_t readSize;

	if (cl == clients.end())
		throw runtime_error("Client find Error");
	bzero(msg, sizeof(msg));

	if((readSize = read(fd, msg, MSG_SIZE -1)) < 0)
	{
		disConnection(fd);
		return 1;
	}
	string& res=clients[fd]->buf;
	if (readSize == 0)
		return (0);
	msg[readSize]=0;
	res+=string(msg);
	if(res.size() and res[res.size()-1]=='\n')
	{
		stringstream ss(res);
		res="";
		string tmp;
		while (getline(ss, tmp))
			if(cmds->doCommand(tmp, fd))
				return 1;
	}
	return 0;
}

Client* Server::find_by_nickname(string nick)
{
	for(client_iterator it=clients.begin(); it!=clients.end(); it++)
		if(nick==it->second->getNickName())
			return it->second;
	return 0;
}

//getter
map<string, Channel*> &Server::getChannels(void){return channels;}
map<int, Client*> &Server::getClients(void){return clients;}
Bot* Server::getBot(){return bot;}
const string &Server::getPasswd(void) const {return passwd;}
Client* Server::getClientByName(const string name)
{
	for(client_iterator it = clients.begin(); it != clients.end(); it ++)
	{
		if (it->second->getNickName() == name)
			return (it->second);
	}
	return (NULL);
}
