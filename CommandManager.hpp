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

#ifndef COMMANDMANAGER
#define COMMANDMANAGER 

class CommandManager;

#include "headers.h"
#include "types.h"
#include "Server.hpp"

using namespace std;

class CommandManager
{
	private :
		// instance
		Server *server;
		map<string, Channel*> &channels;
		map<int, Client*> &clients;

		 //private method
		vector<string>parseCmd(string cmd, char del=' ');
		vector<string>parseCmdWhiteSpace(string cmd);

		int forbidden_char_check(string& s);
		int register_process(vector<string>& parsed, Client& client);
		int command_nick(Client& client, vector<string>& parsed);
		int command_user(Client& client, vector<string>& parsed);
		int command_names(Client& client, vector<string>& parsed);
		int command_join(Client& client, string &line);
		//vector<string>& parsed);
		int command_privmsg(Client& client, vector<string>& parsed, string &line);
		int command_mode(Client& client, vector<string>& parsed);
		int command_who(Client& client, vector<string>& parsed);
		int command_part(Client& client, vector<string>& parsed);
		int command_quit(Client& client, vector<string>& parsed);
		int command_kick(Client& client, vector<string>& parsed);
		int command_sendfile(Client &client, vector<string>& parsed);

	public :
		explicit CommandManager(Server *server);
		~CommandManager();


		//method
		int doCommand(string line, int fd);

};

#endif /* ifndef COMMANDMANAGER */
