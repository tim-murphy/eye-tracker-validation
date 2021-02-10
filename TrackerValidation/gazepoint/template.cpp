//////////////////////////////////////////////////////////////////////////////////////
// template.cpp - Example using the GPClient class
// Written in 2013 by Gazepoint www.gazept.com
//
// To the extent possible under law, the author(s) have dedicated all copyright 
// and related and neighboring rights to this software to the public domain worldwide. 
// This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along with this 
// software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include <iostream>

#include "..\include\GPClient.h"

int _tmain(int argc, _TCHAR* argv[])
{
	GPClient client;
	client.client_connect();

	client.send_cmd("<SET ID=\"ENABLE_SEND_TIME\" STATE=\"1\" />");
	client.send_cmd("<SET ID=\"ENABLE_SEND_POG_FIX\" STATE=\"1\" />");
	client.send_cmd("<SET ID=\"ENABLE_SEND_DATA\" STATE=\"1\" />");

	do 
	{
		string rxstr = client.get_rx_latest();

		if (rxstr != "") 
		{
			cout << rxstr.c_str() << "\n";
		}
	}
	while (1);

	return 0;
}

