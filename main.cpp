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

#include "headers.h"
#include "Server.hpp"

using namespace std;

int main(int ac, char *av[])
{
	Server server;

	if (ac != 3)
	{	
	 	cerr << "Invalid number of argment" << endl;
	 	return (1);
	}
	if (server.init(av))
		perror("Invalid server init"),exit(1);
	try
	{
		server.newSocket();
		cout << "Server is running on " << av[1] << "/" << av[2] << endl;
		server.start();
	}
	catch(runtime_error &e)
	{
		cout << BOLDRED << e.what() << RESET<<endl;
	}
	return (0);
}
