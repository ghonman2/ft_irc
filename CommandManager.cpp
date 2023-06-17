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

#include "CommandManager.hpp"
#include"macros.hpp"

CommandManager::CommandManager(Server *server) : server(server), channels(server->getChannels()), clients(server->getClients())
{}

CommandManager::~CommandManager(){}

vector<string> CommandManager::parseCmd(string cmd, char del)
{
    vector<string> cmds;
    stringstream ss(cmd);
    string tmp;

    while (getline(ss, tmp, del)){
		if(tmp.length() == 0)
			continue ;
		if(tmp[tmp.size()-1] == '\r')
			tmp.resize(tmp.size()-1);
        cmds.push_back(tmp);
    }
	for (size_t i = 0 ; i < cmds.size(); i ++)
		trim(cmds[i]);
    return cmds;
}

vector<string> CommandManager::parseCmdWhiteSpace(string cmd)
{
	stringstream ss(cmd);
	string tmp;
	vector<string> cmds;

	while (ss >> skipws >> tmp)
	{
		if (tmp.length() == 0)
			continue ;
		cmds.push_back(tmp);
	}
	return (cmds);
}

int CommandManager::doCommand(string line, int fd)
{
	// the line should end with '\n' or '\r', must not truncated and must not concatted
	int ret = 0;
	cout << CYAN << fd << " << " << line << '\n' << RESET << std::flush;
	
	Client& client=*clients[fd];
	vector<string> parsed=parseCmdWhiteSpace(line);
	if (line.length() == 0 || parsed.size() == 0)
		return (0);
	if(client.status != REGISTER)
		return register_process(parsed, client);
	else if(parsed[0] == "PING")
		client.write("PONG "+line.substr(5));
	else
	{
		if(parsed[0]=="JOIN")
			ret = command_join(client, line);
		else if(parsed[0]=="PRIVMSG" or parsed[0]=="NOTICE")
			ret = command_privmsg(client, parsed, line);
		else if(parsed[0]=="MODE")
			ret = command_mode(client, parsed);
		else if(parsed[0]=="WHO")
			ret = command_who(client, parsed);
		else if(parsed[0]=="PART")
			ret = command_part(client, parsed);
		else if(parsed[0]=="QUIT")
			ret = command_quit(client, parsed);
		else if (parsed[0]=="KICK")
			ret = command_kick(client, parsed);
		else if (parsed[0] == "SENDFILE")
			ret = command_sendfile(client, parsed);
	}
	return (ret);
}

int CommandManager::command_quit(Client& client, vector<string>& parsed)
{
	if(parsed.size() < 2)
	{
		client.write(ERR_NEEDMOREPARAMS(client.getNickName(), "QUIT"));
		return 1;
	}
	for(set<string>::iterator it=client.channels.begin(); it != client.channels.end(); it++)
		channels[*it]->quit(&client, "");
	client.channels.clear();
	return (1);
}

int CommandManager::command_who(Client& client, vector<string>& parsed)
{
	// https://modern.ircdocs.horse/#who-message
	if(parsed.size() < 2)
	{
		client.write(ERR_NEEDMOREPARAMS(client.getNickName(), "WHO"));
		return 0;
	}

	// in our serer, the mask only can be the channel
	if(parsed[1][0]=='#')
	{
		if(channels.find(parsed[1]) == channels.end())
		{
			client.write(ERR_NOSUCHCHANNEL(client.getNickName(), parsed[1]));
			return 0;
		}
		Channel *channel = channels[parsed[1]];
		map<Client*, int>&clients=channel->getClients();
		for(map<Client*, int>::iterator it=clients.begin(); it!=clients.end(); it++)
			client.write(RPL_WHOREPLY(client.getNickName(), parsed[1], it->first->getUserName(), it->first->getHostName(), SERVER_NAME, 
						it->first->getNickName(), it->first->getRealName()));
		client.write(RPL_ENDOFWHO(client.getNickName(), parsed[1]));
	}
	return 0;
}

int CommandManager::command_part(Client& client, vector<string>& parsed)
{
	// https://modern.ircdocs.horse/#part-message
	Channel *ch;

	if(parsed.size() < 2)
	{
		client.write(ERR_NEEDMOREPARAMS(client.getNickName(), "PART"));
		return 0;
	}

	vector<string> vchannels = parseCmd(parsed[1], ',');
	for(size_t i=0; i < vchannels.size(); i++)
	{
		vchannels[i] = trim(vchannels[i]);
		if(vchannels[i].length() == 0 or channels.find(vchannels[i]) == channels.end())
		{
			client.write(ERR_NOSUCHCHANNEL(client.getNickName(), vchannels[i]));
			continue;
		}
		ch = channels[vchannels[i]];
		channels[vchannels[i]]->part(&client);
		client.channels.erase(vchannels[i]);
		if (ch && ch->getClients().size() == 0)
		{
			channels.erase(ch->getName());
			delete ch;
		}
	}
	return 0;
}

int CommandManager::command_nick(Client& client, vector<string>& parsed)
{
	// https://modern.ircdocs.horse/#nick-message
	if(parsed.size() != 2)
	{
		client.write(ERR_NEEDMOREPARAMS(client.getNickName(), "USER"));
		return 1;
	}
	if(forbidden_char_check(parsed[1]))
	{
		client.write(ERR_ERRONEUSNICKNAME(client.getNickName(), parsed[1]));
		return 1;
	}
	if(server->find_by_nickname(parsed[1]))
	{
		client.write(ERR_NICKNAMEINUSE(parsed[1]));
		return 0;
	}
	return (client.setNickName(parsed[1]));
}

int CommandManager::command_sendfile(Client &client, vector<string> &parsed)
{	
	if (parsed.size() != 2)
	{
		client.write(ERR_NEEDMOREPARAMS(client.getNickName(), "USER"));
		return (0);
	}
	if (parsed[1].find("../") != string::npos)
	{
		client.write(ERR_NOSUCHFILE(client.getNickName(), parsed[1]));
		return (0);
	}

	ifstream fin("./filetranser/" + parsed[1]);
	if (fin.fail())
	{
		client.write(ERR_NOSUCHFILE(client.getNickName(), parsed[1]));
		return (0);
	}
	string tmp;
	string msg = "";
	while (getline(fin, tmp))
		msg += tmp + "\n";
	client.write(msg);
	fin.close();
	return (0);
}

int CommandManager::command_user(Client& client, vector<string>& parsed)
{
	// https://modern.ircdocs.horse/#user-message
	if(parsed.size() < 5)
	{
		client.write(ERR_NEEDMOREPARAMS(client.getNickName(), "USER"));
		return 0;
	}

	string realname="";
	for(size_t i=4; i<parsed.size(); i++)
		realname+=" "+parsed[i];
	if(realname[0]==':')
		realname=realname.substr(2);
	else
		realname=realname.substr(1);

	client.setUserName(parsed[1]);
	client.setRealName(realname);
	return 0;
}

int CommandManager::command_names(Client& client, vector<string>& parsed)
{
	string users="";

	if(parsed.size()==1) // all channels
	{
		for(map<string, Channel*>::iterator chan_it=channels.begin(); chan_it!=channels.end(); chan_it++)
		{
			Channel& chan=*(chan_it->second);
			for(map<Client*, int>::iterator it=chan.getClients().begin(); it!=chan.getClients().end(); it++)
				users+=(chan.getClients()[it->first] ? " @":" ")+ it->first->getNickName();
			client.write(RPL_NAMREPLY(client.getNickName(), chan.getName(), users.substr(1)));
			client.write(RPL_ENDOFNAMES(client.getNickName(), chan.getName()));
		}
	}
	else // particular channels
	{
		vector<string> parsed_chan=parseCmd(parsed[1], ',');
		for(size_t i=0; i<parsed_chan.size(); i++)
		{
			for(map<Client*, int>::iterator it=channels[parsed_chan[i]]->getClients().begin(); it!=channels[parsed_chan[i]]->getClients().end(); it++)
				users+=(it->second ? " @":" ")+ it->first->getNickName();
			client.write(RPL_NAMREPLY(client.getNickName(), parsed_chan[i], users.substr(1)));
			client.write(RPL_ENDOFNAMES(client.getNickName(), parsed_chan[i]));
		}
	}
	return 0;
}

int CommandManager::command_join(Client& client, string &line)
{
	// https://modern.ircdocs.horse/#join-message
	string cmd = line.substr(line.find("JOIN") + 5);
	vector<string> parsed = parseCmd(cmd, ',');
	if(parsed.size() < 1)
	{
		client.write(ERR_NEEDMOREPARAMS(client.getNickName(), parsed[0]));
		return (0);
	}
	for(size_t i=0; i<parsed.size(); i++)
	{
		string& chan_name=parsed[i];
		if(channels.find(chan_name) == channels.end())
		{
			// create new channel
			if (chan_name.size() > 1 && chan_name[1] == ' ')
				chan_name = "#" + chan_name.substr(2);
			if(chan_name[0]!='#' or chan_name.find("\x07") != string::npos or isInWhiteSpace(chan_name))
			{
				client.write(ERR_BADCHANMASK(chan_name));
				continue;
			}
			Channel *chan=new Channel(chan_name);
			chan->addClient(&client);
			chan->setOperator(&client);
			channels[chan_name]=chan;
		}
		else
		{
			// join the channel already exists
			if(channels[chan_name]->addClient(&client))
			{
				client.write(ERR_CHANNELISFULL(client.getNickName(), chan_name));
				return 0;
			}
		}
		// welcome messages 
		vector<string> tmp(2);
		tmp[0]="NAMES", tmp[1]=chan_name;
		command_names(client, tmp);
		client.channels.insert(chan_name);
	}
	return 0;
}

int CommandManager::command_privmsg(Client& client, vector<string>& parsed, string &line)
{
	// https://modern.ircdocs.horse/#privmsg-message
	if(parsed.size() < 2)
	{
		client.write(ERR_NEEDMOREPARAMS(client.getNickName(), "PRIVMSG"));
		return 0;
	}
	string msg="";
	msg = line.substr(line.find(":") + 1);
	if (msg == line)
		msg = line.substr(line.find(parsed[1]) + parsed[1].length() + 1);
	if (msg.size() > 0)
		msg.resize(msg.size() - 1);
	if (msg.size() == 0)
	{
		client.write(ERR_NEEDMOREPARAMS(client.getNickName(), "PRIVMSG"));
		return 0;
	}
	vector<string> targets=parseCmd(parsed[1], ',');
	for(size_t i=0; i<targets.size(); i++)
	{
		if(targets[i][0]=='#') // channel message
		{
			if(channels.find(targets[i]) == channels.end())
			{
				client.write(ERR_NOSUCHNICK(client.getNickName(), targets[i]));
				break;
			}
			channels[targets[i]]->privmsg(&client, msg);
			server->getBot()->check_command(msg, channels[targets[i]]);
		}
		else // private message
		{
			Client* target=server->find_by_nickname(targets[i]);
			if(!target)
			{
				client.write(ERR_NOSUCHNICK(client.getNickName(), targets[i]));
				break;
			}
			target->write(":"+client.getNickName()+"!"+client.getUserName()+"@"+client.getHostName()+" PRIVMSG "+target->getNickName()+" :"+msg);
		}
	}
	return 0;
}

int CommandManager::command_mode(Client& client, vector<string>& parsed)
{
	//	https://modern.ircdocs.horse/#mode-message
	if(parsed.size() < 2)
	{
		client.write(ERR_NEEDMOREPARAMS(client.getNickName(), "MODE"));
		return 0;
	}

	string& chan_name=parsed[1];
	if(chan_name == client.getNickName()) // there is no server operator
			client.write("MODE "+client.getNickName()+" :+i");	
	else if(parsed.size()==2) // without modestring
	{
		if(channels.find(chan_name) == channels.end())
			client.write(ERR_NOSUCHCHANNEL(client.getNickName(), chan_name));
		else if(channels[chan_name]->getClients()[&client])
			client.write(RPL_CHANNELMODEIS(client.getNickName(), chan_name, "+io"));
		else
			client.write(RPL_CHANNELMODEIS(client.getNickName(), chan_name, "+i"));
	}
	else if(parsed.size()==4) // with modestring
	{
		if (!channels[chan_name])
			client.write(ERR_NOSUCHNICK(client.getNickName(), chan_name));
		else if(!channels[chan_name]->getClients()[&client])
			client.write(ERR_CHANOPRIVSNEEDED(client.getUserName(), chan_name));
		else if(!channels[chan_name]->getClientByName(parsed[3]))
			client.write(ERR_NOSUCHNICK(client.getNickName(), parsed[3]));
		else if(parsed[2]=="+o")
		{
			channels[chan_name]->setOperator(channels[chan_name]->getClientByName(parsed[3]));
			channels[chan_name]->broadcast(MODE_BROADCAST(chan_name, "+o", parsed[3]));
		}
		else if(parsed[2]=="-o")
		{
			channels[chan_name]->setNotOperator(channels[chan_name]->getClientByName(parsed[3]));
			channels[chan_name]->broadcast(MODE_BROADCAST(chan_name, "-o", parsed[3]));
		}
	}
	return 0;
}

int CommandManager::forbidden_char_check(string& s)
{
	string forbidded_chars=" ,*?!@.:";
	if(s[0]=='$' or s[0]==':' or s[0]=='#')
		return 1;
	for(size_t i=0; i<s.length(); i++)
		if(forbidded_chars.find(s[i]) != string::npos)
			return 1;
	return 0;
}

int CommandManager::register_process(vector<string>& parsed, Client& client)
{
	if(client.status == NOT_REGISTER)
	{
		if(parsed.size() == 2 and parsed[0] == "CAP" and parsed[1] == "LS")
			client.status=REG_CAP;
	}
	else if(client.status == REG_CAP)
	{
		if(parsed[0] == "PASS")
		{
			if(parsed.size() != 2)
			{
				client.write(ERR_NEEDMOREPARAMS(SERVER_NAME, parsed[0]));
				return 1;
			}
			else if(parsed[1] == server->getPasswd())
				client.status=REG_PASS;
			else
			{
				client.write(ERR_PASSWDMISMATCH(SERVER_NAME));
				return 1;
			}
		}
	}
	else if(client.status == REG_PASS)
	{
		if (parsed.empty())
			return (1);
		else if(parsed[0] == "NICK")
			command_nick(client, parsed);
		else if(parsed[0] == "USER")
			command_user(client, parsed);
		else
			client.write(ERR_NOTREGISTERED(client.getNickName()));
		// auth complete
		if(client.getNickName() != "" and client.getUserName() != "")
		{
			client.status=REGISTER, client.setRegistered();
			cout << client.getNickName() << "!" << client.getUserName() << "@" << client.getHostName() << " is succesfully registered\n" << flush;
			client.write(RPL_WELCOME(client.getNickName(), SERVER_NAME, client.getNickName(), client.getUserName(), client.getHostName()));
			client.write(RPL_YOURHOST(client.getNickName(), SERVER_NAME, SERVER_VER));
			client.write(RPL_CREATED(client.getNickName(), CREATE_TIME));
			client.write(RPL_MYINFO(client.getNickName(), SERVER_NAME, SERVER_VER, "i", "io", ""));
			client.write(RPL_LSUPPORT(client.getNickName(), "tjoowpark"));
		}
	}
	return 0;
}

int CommandManager::command_kick(Client& client, vector<string>& parsed)
{
	if (parsed.size () < 3)
	{
		client.write(ERR_NEEDMOREPARAMS(client.getNickName(), "KICK"));
		return (0);
	}

	Client *target = NULL;
	string& chan_name=parsed[1];
	
	if (!channels[chan_name])
		client.write(ERR_NOTONCHANNEL(client.getNickName(), chan_name));
	else
		target = channels[chan_name]->getClientByName(parsed[2]);
	
	if(channels.find(chan_name) == channels.end())
		client.write(ERR_NOSUCHCHANNEL(client.getNickName(), chan_name));
	else if(!channels[chan_name]->getClients()[&client])
		client.write(ERR_CHANOPRIVSNEEDED(client.getUserName(), chan_name));
	else if(client.channels.find(chan_name)==client.channels.end())
		client.write(ERR_NOTONCHANNEL(client.getNickName(), chan_name));
	else if(!target || target->channels.find(chan_name) == target->channels.end())
		client.write(ERR_USERNOTINCHANNEL(client.getNickName(), parsed[2], chan_name));
	else
	{
		channels[chan_name]->broadcast(":"+client.getNickName()+"!"+client.getUserName()+"@"+client.getHostName()+" KICK "+chan_name+" "+parsed[2]);
		channels[chan_name]->part(target);
		channels[chan_name]->eraseClient(target);
	}
	return 0;
}
