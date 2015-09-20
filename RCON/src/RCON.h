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
	RCON(short port, std::string bindip, std::string password);
	~RCON();
	std::vector<Client*> clients;
	std::thread* sockthread;
	SOCKET sockid;
	bool isHosted;
	std::string password;
	static void Loop(RCON* r);
	bool send(int id, std::string msg);
	bool send(Client* c, std::string msg);
	bool sendex(int id, const char* format, ...);
	bool sendex(Client* c, const char* format, ...);
	void DisconnectClient(Client* c);
	int Broadcast(std::string message);
	int Broadcastex(const char* format, ...);
	static void OnRecv(Client* c, std::string msg);
	void OnClientDisconnect(Client* c);
	Client* GetClient(int id);
};

const std::string commands[][3] =
{
	{"help", "Shows the available list of commands", "[optional:command name, empty for all]"},
	{"exit", "Disconnects your current RCON session", ""},
	{"kick", "Kicks a player from the server", "[playerid] [optional:reason]"},
	{"ban", "Bans a player from the server", "[playerid] [optional:reason]"},
	{"banip", "Bans the given IP from the server", "[IPv4]"},
	{"unbanip", "Unbans the given IP from the server", "[IPv4]"},
	{"players", "Lists the connected players with basic info", ""},
	{ "servername", "Gets/Sets the server name", "get: none, set: [server name]" },
	{ "maxplayers", "Gets/Sets the maxplayers", "get: none, set: [slots]" },
	{ "serverpassword", "Gets/Sets the server password", "get: none, set: [password]" },
	{ "gamemodetext", "Gets/Sets the game mode text", "get: none, set: [mode name]" },
	{ "timerate", "Gets/Sets the time rate", "get: none, set: [rate]" },
	{ "hour", "Gets/Sets the game hour", "get: none, set: [new hour]" },
	{ "minute", "Gets/Sets the game minute", "get: none, set: [new minute]" },
	{ "weather", "Gets/Sets the game weather", "get: none, set: [new weather]" },
	{ "gravity", "Gets/Sets the game gravity", "get: none, set: [new gravity]" },
	{ "gamespeed", "Gets/Sets the game speed", "get: none, set: [new gamespeed]" },
	{ "waterlevel", "Gets/Sets the game water level", "get: none, set: [new waterlevel]" },
	{ "maxheight", "Gets/Sets the game max height", "get: none, set: [new maxheight]" },
	{ "killcmddelay", "Gets/Sets the game kill command delay", "get: none, set: [new killcmddelay]" },
	{ "vfrheight", "Gets/Sets the game Vehicle Force Respawn height", "get: none, set: [new height]" },
	{"listclients", "Lists the connected RCON clients", ""}
};

#endif