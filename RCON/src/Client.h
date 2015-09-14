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

#ifndef __RCLIENT_H
#define __RCLIENT_H

#include "main.h"
class RCON;

class Client
{
	SOCKET ssock;
	SOCKET csock;
	std::thread* sockthread;
public:
	sockaddr_in* clientsock;
	RCON* _rcon;
	bool isConnected;
	Client(SOCKET s, SOCKET c, sockaddr_in* cs);
	~Client();
	bool Send(std::string msg);
	bool Sendex(const char* format, ...);
	static void Loop(Client* c);
	void OnDisconnect();
	bool isIdentified;
private:
	bool send(const char* msg);
};

#define ipaddr(c) std::string(inet_ntoa(c->clientsock->sin_addr))

#endif