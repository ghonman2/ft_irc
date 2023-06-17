#include"Bot.hpp"

Bot::Bot()
{
    bot_client=new Client(-1, "server", -1);
    bot_client->setNickName("bot");
    bot_client->setRealName("server_bot");
    bot_client->setUserName("server_bot");

    srand(time(0));
}

Bot::~Bot()
{
    delete bot_client;
}

vector<string> Bot::parseCmd(string line)
{
	vector<string> ret;
	stringstream ss(line);
	string tmp;

	while (1)
	{
		if (ss >> tmp)
			ret.push_back(tmp);
		else 
			break ;
	}
	return (ret);
}

int Bot::check_command(string s, Channel* chan)
{
    vector<string> cmd = parseCmd(s);
	if (cmd.empty() || cmd[0] != "@봇")
		return (0);
	else if (cmd.size() > 1 && cmd[1] == "뭐먹")
        what_food(chan);
	else 
		chan->privmsg(bot_client, "\'@봇 뭐먹\' 이라고 말해주세요");
    return 0;
}

int Bot::what_food(Channel* chan)
{
    string ret;

    switch(rand()%22)
    {
        case 0:
            ret = "양꼬치!!";
            break;
        case 1:
            ret = "피탕!!";
            break;
        case 2:
            ret = "피맥!!";
            break;
        case 3:
            ret = "감자탕!!";
            break;
        case 4:
            ret = "치킨!!";
            break;
        case 5:
            ret = "부리또!!";
            break;
        case 6:
            ret = "김볶밥!!";
            break;
        case 7:
            ret = "소고기!!";
            break;
        case 8:
            ret = "햄버거!!";
            break;
        case 9:
            ret = "부찌!!";
            break;
        case 10:
            ret = "치킨스테이크!!";
            break;
        case 11:
            ret = "아웃백!!";
            break;
        case 12:
            ret = "빕스!!";
            break;
        case 13:
            ret = "황금볶음밥!!";
            break;
        case 14:
            ret = "마라탕!!";
            break;
        case 15:
            ret = "슈하스코!!";
            break;
        case 16:
            ret = "떡볶이!!";
            break;
        case 17:
            ret = "학식!!";
            break;
        case 18:
            ret = "돈까스!!";
            break;
        case 19:
            ret = "곱창!!";
            break;
        case 20:
            ret = "삼계탕!!";
            break;
        case 21:
            ret = "짜장면!!";
            break;
    }
    chan->privmsg(bot_client, ret);
    return 0;
}
