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

#ifndef __RSQFUNCS_H
#define __RSQFUNCS_H

#include "main.h"

namespace SquirrelFuncs
{
	void RegisterFunctions(HSQUIRRELVM* v);
	SQInteger RCON_Send(HSQUIRRELVM v); // RCON_Send(int clientid, string text);
	SQInteger RCON_Broadcast(HSQUIRRELVM v); // RCON_Broadcast(string text);
	SQInteger RCON_GetClients(HSQUIRRELVM v); // RCON_GetClients();
}

void register_global_func(HSQUIRRELVM vm, const char *name, SQFUNCTION function);

#endif