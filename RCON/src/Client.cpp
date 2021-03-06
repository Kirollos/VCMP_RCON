/*
	Copyright 2016 Kirollos

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
#include "Client.h"

Client::Client(SOCKET s, SOCKET c, sockaddr_in cs, RCON* r)
{
	this->_rcon = r;
	ssock = s;
	csock = c;
	clientsock = new sockaddr_in();
	*clientsock = cs;
	this->isConnected = true;
	this->sockthread = new std::thread(Client::Loop, this);
	this->sockthread->detach();
	VCMP_PF->LogMessage("[RCON]: A client has connected! (IP: %s)", ipaddr(this).c_str());
	char* version = new char[20];
	GetRVersion(version);
	this->Sendex("VCMP-RCON v%s", version);
	delete version;
	this->isIdentified = false;
}

Client::~Client()
{
	if (this->isConnected)
	{
		this->isConnected = false;
		Sleep(250);
#ifdef _WIN32
		closesocket(this->csock);
#else
		close(this->csock);
#endif
	}
	if (this->sockthread->joinable())
		this->sockthread->join();
}

bool Client::Send(std::string msg)
{
	if (!msg.empty()) {
		if (msg.find("\n") == std::string::npos && msg.find("\r\n") == std::string::npos)
		{
			msg.append("\r\n"); // Some clients do not like \n alone
		}
		//return sendto(this->csock, msg.c_str(), msg.length(), 0, this->clientsock, sizeof(this->clientsock)) != -1;
	}
	if (!this->send(msg.c_str()))
	{
		this->OnDisconnect();
		return false;
	}
	return true;
}

bool Client::send(const char* msg)
{
	return sendto(this->csock, msg, strlen(msg), 0, (const sockaddr*) this->clientsock, sizeof(this->clientsock)) != -1;
}

bool Client::Sendex(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	char* formatted = new char[1024];
	vsprintf(formatted, format, args);

	bool retval = this->Send(std::string(formatted));

	va_end(args);
	delete formatted;
	return retval;
}

bool Client::Disconnect()
{
	isConnected = false;
	for (std::vector<Client*>::iterator it = this->_rcon->clients.begin(); it != this->_rcon->clients.end(); it++)
	{
		if (*it == this)
		{
			this->_rcon->clients.erase(it);
			break;
		}
	}
	return shutdown(this->csock, SD_BOTH) == 0;
}

void Client::Loop(Client* c)
{
	if (!c->isConnected) return;
	std::string _data;
	while (true)
	{
		if (!c->isConnected) break;
		int recvsize;
		while (true)
		{
			recvsize = 0;
			char r = NULL;
			socklen_t clientsock_len = sizeof(c->clientsock);
			if ((recvsize = recvfrom(c->csock, &r, 1, 0, (sockaddr*) c->clientsock, &clientsock_len)) > 0)
			{
				_data += r;
			}
			else {
				c->isConnected = false;
				break;
			}

			if (r == '\n')
				break;
			else
				continue;
		}
		RCON::OnRecv(c, _data);
		_data.clear();
		continue;
	}
	c->isConnected = false;
	c->OnDisconnect();
#ifdef _WIN32
	closesocket(c->csock);
#else
	close(c->csock);
#endif
	return;
}

void Client::OnDisconnect()
{
	this->_rcon->OnClientDisconnect(this);
}

int Client::GetID()
{
	for (int i = 0; i < (int)this->_rcon->clients.size(); i++)
	{
		if (this->_rcon->clients[i] == this)
			return i;
	}
	return -1;
}

int Client::GetID(Client* c)
{
	for (int i = 0; i < (int)c->_rcon->clients.size(); i++)
	{
		if (c->_rcon->clients[i] == c)
			return i;
	}
	return -1;
}
