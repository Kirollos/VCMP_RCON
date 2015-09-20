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
#include "SquirrelFunctions.h"

namespace SquirrelFuncs
{
	void RegisterFunctions(HSQUIRRELVM* v)
	{
#define REGISTERSQFUNCTION(name) register_global_func(*v, #name, SquirrelFuncs::name)
		REGISTERSQFUNCTION(RCON_Send);
		REGISTERSQFUNCTION(RCON_Broadcast);
		REGISTERSQFUNCTION(RCON_GetClients);
	}

	SQInteger RCON_Send(HSQUIRRELVM v) // RCON_Send(int clientid, string text);
	{
		SQInteger clientid;
		const SQChar* text;

		sq_getinteger(v, 2, &clientid);
		sq_getstring(v, 3, &text);

		Client* c = rcon->GetClient((int)clientid);
		if (c == nullptr)
		{
			sq_pushbool(v, false);
			return 1;
		}
		sq_pushbool(v, c->Send(std::string((char*) text)));
		return 1;
	}

	SQInteger RCON_Broadcast(HSQUIRRELVM v) // RCON_Broadcast(string text);
	{
		const SQChar* text;

		sq_getstring(v, 2, &text);
		SQInteger count = 0;
		count = rcon->Broadcast(std::string((char*)text));
		sq_pushinteger(v, count);
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
		sq_newarray(v, 0); // main array
		
		for (Client* c : rcon->clients)
		{
			// client array
			sq_newarray(v, 0); // client array
			sq_pushstring(v, (const SQChar*)ipaddr(c).c_str(), -1); // ip
			sq_arrayappend(v, -2);
			sq_pushbool(v, (SQBool)c->isIdentified); // identified
			sq_arrayappend(v, -2); // append to client array
			sq_arrayappend(v, -2); // append client array to main array
		}
		return 1;
	}
}

void register_global_func(HSQUIRRELVM vm, const char *name, SQFUNCTION function)
{
	sq_pushroottable(vm);
	sq_pushstring(vm, (const SQChar*)name, -1);
	sq_newclosure(vm, function, 0);
	sq_createslot(vm, -3);
	sq_pop(vm, 1);
}