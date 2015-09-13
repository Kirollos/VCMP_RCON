/*
	Copyright 2015 Kirollos

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#pragma once

#ifndef __RCON_H
#define __RCON_H

#ifdef _WIN32

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
typedef int SOCKET;
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#define Sleep(x)        sleep(x/1000);
#define SD_BOTH         SHUT_RDWR
#define SOCKET_ERROR    (-1)
#define INVALID_SOCKET  (-1)
#endif
#include <thread>
#include <vector>

#include "main.h"
#include "Client.h"

class RCON
{
	sockaddr_in serversock;
	short _port;
	std::string _bindip;
	bool sbind();
	void slisten();
#ifdef _WIN32
	WSAData _WSADATA;
#endif
public:
	RCON(short port, std::string bindip = "0.0.0.0");
	~RCON();
	std::vector<Client*> clients;
	std::thread* sockthread;
	SOCKET sockid;
	bool isHosted;
	static void Loop(RCON* r);
	bool send(int id, std::string msg);
	bool send(Client* c, std::string msg);
	bool sendex(int id, const char* format);
	bool sendex(Client* c, const char* format);
	static void OnRecv(Client* c, std::string msg);
	void OnClientDisconnect(Client* c);
};

#endif