#pragma once

#define SERVER_NAME "TJOOWPARK"
#define CREATE_TIME "20230303"
#define SERVER_VER "v1.1"

#include<string>
using namespace std;

string RPL_WELCOME(string client, string servername, string nick, string user, string host)
{
    return "001 "+client+" :Welcome to the "+servername+" Network, "+nick+"!"+user+"@"+host; 
}

string RPL_YOURHOST(string client, string servername, string version)
{
    return "002 "+client+" :Your host is "+servername+", running version "+version;
}

string RPL_CREATED(string client, string datetime)
{
    return "003 "+client+" :This server was created "+datetime;
}

string RPL_MYINFO(string client, string servername, string version, string avail_mode, string avail_charmode, string charmode_param)
{
    return "004 "+client+" "+servername+" "+version+" "+avail_mode+" "+avail_charmode+" :"+charmode_param;
}

string RPL_LSUPPORT(string client, string token)
{
    return "005 "+client+" "+token+" :are supported by this server";
}

string ERR_NEEDMOREPARAMS(string client, string command)
{
    return "461 "+client+" "+command+" :Not enough parameters";
}

string ERR_PASSWDMISMATCH(string client)
{
    return "464 "+client+" :Password incorrect";
}

string ERR_CHANNELISFULL(string client, string channel)
{
    return "471 "+client+" "+channel+" :Cannot join channel (+l)";
}

string ERR_NICKNAMEINUSE(string client)
{
    return "433 * "+client+" :Nickname is already in use";
}

string RPL_NAMREPLY(string nick, string channel, string users)
{
    return "353 "+nick+" = "+channel+" :"+users;
}

string RPL_ENDOFNAMES(string nick, string channel)
{  
    return "366 "+nick+" "+channel+" :End of /NAMES list.";
}

string RPL_NOTOPIC(string client, string channel)
{
    return "331 "+client+" "+channel+" :No topic is set";
}

string RPL_CHANNELMODEIS(string client, string channel, string modestring)
{
    return "324 "+client+" "+channel+" :"+modestring;
}

string RPL_WHOREPLY(string client, string channel, string username, string hostname, string server, string nick, string realname)
{
    return "352 "+client+" "+channel+" "+username+" "+hostname+" "+server+" "+nick+" H :0 "+realname;
}

string RPL_ENDOFWHO(string client, string channel)
{
    return "315 "+client+" "+channel+" :End of WHO list";
}

string RPL_ENDOFBANLIST(string client, string channel)
{
    return "368 "+client+" "+channel+" :End of channel ban list";
}

string ERR_NOTREGISTERED(string client)
{
    return "451 "+client+" :You have not registered";
}

string ERR_NOSUCHNICK(string client, string name)
{
    return "401 "+client+" "+name+" :No such nick/channel";
}

string ERR_ERRONEUSNICKNAME(string client, string nick)
{
    return "432 "+client+" "+nick+" :Erroneus nickname";
}

string ERR_NOSUCHCHANNEL(string client, string channel)
{
    return "403 "+client+" "+channel+" :No such channel";
}

string ERR_NOSUCHFILE(string client, string fileName)
{
    return "403 "+client+" "+fileName+" :No such File";
}

string ERR_CHANOPRIVSNEEDED(string client, string channel)
{
    return "482 "+client+" "+channel+" :You're not channel operator";
}

string RPL_UMODEIS(string client, string modes)
{
    return "221 "+client+" "+modes;
}

string MODE_BROADCAST(string channel, string modes, string target)
{
    return "MODE "+channel+" "+modes+" "+target;
}

string ERR_USERNOTINCHANNEL(string client, string nick, string channel)
{
    return "441 "+client+" "+nick+" "+channel+" :They aren't on that channel";
}

string ERR_NOTONCHANNEL(string client, string channel)
{
    return "442 "+client+" "+channel+" :You're not on that channel";
}

string ERR_BADCHANMASK(string channel)
{
    return "476 "+channel+" :Bad Channel Mask";
}

inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v")
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v")
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v")
{
	return ltrim(rtrim(s, t), t);
}

inline int isInWhiteSpace(string &str)
{
	string white = " \t\n\v\f\r";
	for (size_t i = 0 ; i < str.length(); i ++)
	{
		if(white.find(str[i]) != string::npos)
			return (1);
	}
	return (0);
}
