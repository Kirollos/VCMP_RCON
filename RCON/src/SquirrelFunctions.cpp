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
#include "SquirrelFunctions.h"

namespace SquirrelFuncs
{
	void RegisterFunctions(HSQUIRRELVM* v)
	{
#define REGISTERSQFUNCTION(name) register_global_func(*v, #name, SquirrelFuncs::name)
		REGISTERSQFUNCTION(RCON_Send);
		REGISTERSQFUNCTION(RCON_Broadcast);
		REGISTERSQFUNCTION(RCON_GetClients);
		REGISTERSQFUNCTION(RCON_KickClient);
	}

	SQInteger RCON_Send(HSQUIRRELVM v) // RCON_Send(int clientid, string text);
	{
		SQInteger clientid;
		const SQChar* text;

		sqapi->getinteger(v, 2, &clientid);
		sqapi->getstring(v, 3, &text);

		Client* c = rcon->GetClient((int)clientid);
		if (c == nullptr)
		{
			sqapi->pushbool(v, false);
			return 1;
		}
		sqapi->pushbool(v, c->Send(std::string((char*) text)));
		return 1;
	}

	SQInteger RCON_Broadcast(HSQUIRRELVM v) // RCON_Broadcast(string text);
	{
		const SQChar* text;

		sqapi->getstring(v, 2, &text);
		SQInteger count = 0;
		count = rcon->Broadcast(std::string((char*)text));
		sqapi->pushinteger(v, count);
		return 1;
	}

	SQInteger RCON_GetClients(HSQUIRRELVM v) // RCON_GetClients();
	{
		/*
			Returned array will look like this:
			[
			//	[string IP, bool IDENTIFIED]

				["127.0.0.1", true], // first client
				["192.168.1.2", false], // second client
				... // etc..
			]
		*/
		sqapi->newarray(v, 0); // main array
		
		for (Client* c : rcon->clients)
		{
			// client array
			sqapi->newarray(v, 0); // client array
			sqapi->pushstring(v, (const SQChar*)ipaddr(c).c_str(), -1); // ip
			sqapi->arrayappend(v, -2);
			sqapi->pushbool(v, (SQBool)c->isIdentified); // identified
			sqapi->arrayappend(v, -2); // append to client array
			sqapi->arrayappend(v, -2); // append client array to main array
		}
		return 1;
	}

	SQInteger RCON_KickClient(HSQUIRRELVM v) // RCON_KickClient(int clientid);
	{
		SQInteger clientid;

		sqapi->getinteger(v, 2, &clientid);

		Client* c = rcon->GetClient((int)clientid);
		if (c == nullptr)
		{
			sqapi->pushbool(v, false);
			return 1;
		}
		c->Send("[NOTICE]: Your client has been disconnected by the server.");
		c->Disconnect();
		sqapi->pushbool(v, true);
		return 1;
	}
}

void register_global_func(HSQUIRRELVM vm, const char *name, SQFUNCTION function)
{
	sqapi->pushroottable(vm);
	sqapi->pushstring(vm, (const SQChar*)name, -1);
	sqapi->newclosure(vm, function, 0);
	sqapi->newslot(vm, -3, SQFalse);
	sqapi->pop(vm, 1);
}