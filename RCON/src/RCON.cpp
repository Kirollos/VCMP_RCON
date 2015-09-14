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

#include "main.h"
#include "RCON.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <vector>

RCON::RCON(short port, std::string bindip)
{
#ifdef _WIN32
	if (WSAStartup(MAKEWORD(2, 0), &_WSADATA) != 0) {
		WSACleanup();
	}
#endif
	this->sockid = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->sockid == INVALID_SOCKET)
	{
#ifdef _WIN32
		WSACleanup();
#endif
		throw new std::exception("Failed to initialize socket");
	}
	this->_port = port;
	this->_bindip = bindip;
	if (!this->sbind())
		throw new std::exception("Failed to bind to given ip/port");
	this->slisten();

	isHosted = true;
	this->sockthread = new std::thread(RCON::Loop, this);
	this->sockthread->detach();
}

RCON::~RCON()
{
	for (std::vector<Client*>::iterator it = this->clients.begin(); it != this->clients.end(); it++)
	{
		Client* c = *it;
		if (c != nullptr && c->isConnected)
		{
			c->Send("**** Server shutting down!");
			Sleep(100);
			delete c;
		}
	}
	Sleep(750);
#ifdef _WIN32
	closesocket(this->sockid);
	WSACleanup();
#else
	close(this->sockid);
#endif
	this->isHosted = false;
	if (this->sockthread->joinable() == true)
		this->sockthread->join();
	delete this->sockthread;
}

bool RCON::sbind()
{
	this->serversock.sin_family = AF_INET;
	this->serversock.sin_addr.s_addr = (this->_bindip == "0.0.0.0" ? INADDR_ANY : inet_addr(inet_ntoa(*(in_addr*)this->_bindip.c_str())));
	this->serversock.sin_port = htons(this->_port);
	return bind(this->sockid, (struct sockaddr*) &this->serversock, sizeof(this->serversock)) == 0;
}

void RCON::slisten()
{
	listen(this->sockid, /*temp */ 5 /*temp*/);
}

bool RCON::send(int id, std::string msg)
{
	return this->send(this->clients.at((size_t)id), msg);
}

bool RCON::send(Client* c, std::string msg)
{
	return c->Send(msg);
}

bool RCON::sendex(int id, const char* format)
{
	va_list args;
	va_start(args, format);

	char* formatted = new char[1024];
	vsprintf(formatted, format, args);

	bool retval = this->send(this->clients.at((size_t)id), std::string(formatted));

	va_end(args);
	delete formatted;
	return retval;
}

bool RCON::sendex(Client* c, const char* format)
{
	va_list args;
	va_start(args, format);

	char* formatted = new char[1024];
	vsprintf(formatted, format, args);

	bool retval = c->Send(std::string(formatted));

	va_end(args);
	delete formatted;
	return retval;
}

void RCON::Loop(RCON* r)
{
	while (r->isHosted)
	{
		for (std::vector<Client*>::iterator it = r->clients.begin(); it != r->clients.end(); it++)
		{
			if (*it != nullptr && !(*it)->isConnected)
			{
				r->clients.erase(it);
			}
		}
		sockaddr_in csa;
		int csa_len = sizeof(csa);
		SOCKET cid = accept(r->sockid, (sockaddr*) &csa, &csa_len);
		if (cid == -1)
		{
			r->isHosted = false;
			//continue;
			break;
		}
		Client* nc = new Client(r->sockid, cid, &csa);
		nc->_rcon = r;
		r->clients.push_back(nc);
	}
	return;
}

void RCON::OnRecv(Client* c, std::string msg)
{
	if (msg.empty()) return;
	msg.erase(msg.find_last_of("\r\n", msg.length() - 2));

	std::istringstream cmd(msg);
	std::istream_iterator<std::string> linebeg(cmd), lineend;
	std::vector<std::string> params(linebeg, lineend);
	std::string command = params[0];
	params.erase(params.begin());
#define ISCMD(_cmd) if(command == #_cmd) // Laziness can do more
	ISCMD(login)
	{
		if (params.size() == 1)
		{
			if (!c->isIdentified)
			{
				if (params[0] == "tmppass")
				{
					c->Send("Successfully identified!");
					c->isIdentified = true;
				}
				else
				{
					c->Send("Error: Incorrect password!");
				}
			}
			else
			{
				c->Send("Error: You are already identified!");
			}
		}
	}
	else
	{
		if (!c->isIdentified)
		{
			c->Send("Error: You are not authorized to execute this command!");
			VCMP_PF->printf("[RCON]: Client (IP: %s) has attempted to execute \"%s\" before identifying.", ipaddr(c).c_str(), msg.c_str());
			return;
		}

		ISCMD(help)
		{
			// Available commands will be listed in here
		}
		ISCMD(kick)
		{
			if (params.size() < 1)
			{
				c->Send("Syntax: kick [player ID] [optional:reason]");
				return;
			}
			int id = std::stoi(params[0]);
			if (id < 0)
			{
				c->Send("Error: Invalid playerid.");
				return;
			}
			if (!VCMP_PF->IsPlayerConnected(id))
			{
				c->Send("Error: This player is not connected.");
				return;
			}
			std::string reason = "No Reason";
			if (params.size() >= 2)
			{
				reason.clear();
				for (int i = 1; i < params.size(); i++)
				{
					reason += params[i];
					if (i != params.size() - 1)
						reason += " ";
				}
			}
			std::string name, ip;
			char* tmpBuff = new char[512];
			tmpBuff[0] = 0;
			VCMP_PF->GetPlayerName(id, tmpBuff, 512);
			name = std::string(tmpBuff);
			VCMP_PF->GetPlayerIP(id, tmpBuff, 512);
			ip = std::string(tmpBuff);
			//strcpy(tmpBuff, 0);
			delete tmpBuff;
			
			VCMP_PF->SendClientMessage(id, 0xFF0000FF, std::string("You are kicked from this server by RCON admin.").c_str());
			VCMP_PF->SendClientMessage(id, 0xFF0000FF, std::string("Reason: " + reason).c_str());
			for (unsigned i = 0; i < VCMP_PF->GetMaxPlayers(); i++) {
				if (VCMP_PF->IsPlayerConnected(i))
				{
					VCMP_PF->SendClientMessage(id, 0xFF0000FF, std::string("Player " + name + "[" + params[0] + "] has been kicked by RCON admin.").c_str());
					VCMP_PF->SendClientMessage(id, 0xFF0000FF, std::string("Reason: " + reason).c_str());
				}
			}
			c->Send("Successfully kicked " + name + "[" + params[0] + "]{" + ip + "} from the server.");
			VCMP_PF->printf("[RCON]: Client (IP: %s) has kicked %s[%i]{%s} from the server.", ipaddr(c).c_str(), name.c_str(), id, ip.c_str());
			VCMP_PF->KickPlayer(id);
		}
		else
		{
			if (&sqvm == nullptr) return;
			// Send it to squirrel scripts, can be custom command. Who knows?
			int top = sqapi->gettop(sqvm);
			sqapi->pushroottable(sqvm);
			sqapi->pushstring(sqvm, (const SQChar*)"RCON_OnCommand", -1); // RCON_OnCommand(ip, command, params)
			if (SQ_FAILED(sqapi->get(sqvm, -2)))
				return;
			sqapi->pushroottable(sqvm);
			sqapi->pushstring(sqvm, (const SQChar*)ipaddr(c).c_str(), -1); // ip
			sqapi->pushstring(sqvm, (const SQChar*)command.c_str(), -1); // command
			
			std::string paramsstr = "";
			for (int i = 0; i < (int)params.size(); i++)
			{
				paramsstr += params[i];
				if (i != params.size() - 1)
					paramsstr += " ";
			}
			sqapi->pushstring(sqvm, (const SQChar*)paramsstr.c_str(), -1); // params
			
			if(SQ_FAILED(sqapi->call(sqvm, 4, SQFalse, SQFalse)))
				return;

			sqapi->settop(sqvm, top);
		}
	}
}

void RCON::OnClientDisconnect(Client* c)
{
	VCMP_PF->printf("A client has disconnected! (IP: %s)", ipaddr(c).c_str());
}