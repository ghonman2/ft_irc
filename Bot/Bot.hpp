#pragma once

#include "../Util/headers.h"
#include "../Channel/Channel.hpp"
#include "../Client/Client.hpp"

class Bot
{
    private:
        Client *bot_client;
		vector<string> parseCmd(string line);
        int what_food(Channel* chan);
	public:
        int check_command(string s, Channel* chan);
        Bot();
        ~Bot();
};
