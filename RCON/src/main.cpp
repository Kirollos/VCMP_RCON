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
#include "Client.h"

PluginFuncs* VCMP_PF = nullptr;
RCON* rcon = nullptr;

HSQUIRRELVM sqvm;
HSQAPI sqapi;

extern "C"
{
	EXPORT unsigned int VcmpPluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo)
	{
		strcpy(pluginInfo->szName, "RCON");
		pluginInfo->uPluginVer = 0x1000;
		VCMP_PF = pluginFuncs;
		VCMP_PF->printf("Initializing RCON...");
		pluginCalls->OnShutdownServer = OnShutdown;
		pluginCalls->OnInternalCommand = OnInternalCommand;
		rcon = new RCON(1337);
		return 1;
	}
}

int OnInternalCommand(unsigned int uCmdType, const char* pszText)
{
	switch (uCmdType)
	{
	case 0x7D6E22D8:
		OnSquirrelScriptLoad();
		break;

	default:
		break;
	}

	return 1;
}

void OnShutdown()
{
	if (rcon != nullptr)
	{
		delete rcon;
	}
}

void OnSquirrelScriptLoad()
{
	unsigned int size;
	int sqID = VCMP_PF->FindPlugin("SQHost2");
	void** sqExports = VCMP_PF->GetPluginExports(sqID, &size);

	if (sqExports != NULL && size > 0)
	{
		SquirrelImports* sqFuncs = (SquirrelImports*)(*sqExports);
		if (sqFuncs)
		{
			sqvm = *sqFuncs->GetSquirrelVM();
			sqapi = *sqFuncs->GetSquirrelAPI();
		}
	}
}