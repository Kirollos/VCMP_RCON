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
#ifdef _DEBUG
		Sleep(5000); // In need to attach debugger sometimes
#endif
		strcpy(pluginInfo->szName, "RCON");
		pluginInfo->uPluginVer = 0x1000;
		VCMP_PF = pluginFuncs;
		VCMP_PF->printf("Initializing RCON...");
		
		std::string enabled = ConfigUtils::GetConfigValue("rcon_enabled"),
			port = ConfigUtils::GetConfigValue("rcon_port"),
			bindip = ConfigUtils::GetConfigValue("rcon_bindip"),
			password = ConfigUtils::GetConfigValue("rcon_password");

		if (enabled == "ERR_NOT_FOUND")
		{
			ConfigUtils::AppendConfig("rcon_enabled", "false");
			enabled = "false";
		}
		if (port == "ERR_NOT_FOUND")
		{
			ConfigUtils::AppendConfig("rcon_port", "1337");
			port = "1337";
		}
		if (bindip == "ERR_NOT_FOUND")
		{
			ConfigUtils::AppendConfig("rcon_bindip", "0.0.0.0");
			bindip = "0.0.0.0";
		}
		if (password == "ERR_NOT_FOUND")
		{
			ConfigUtils::AppendConfig("rcon_password", "plschange");
			password = "plschange";
		}

		if (!ConfigUtils::GetBool(enabled))
			return 1;

		if (ConfigUtils::GetInt(port) == 0)
			port = "1337";
		if (bindip.empty())
			bindip = "0.0.0.0";
		if (password.empty())
			password = "plschange";
		
		pluginCalls->OnShutdownServer = OnShutdown;
		pluginCalls->OnInternalCommand = OnInternalCommand;
		rcon = new RCON(ConfigUtils::GetInt(port), bindip, password);
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

namespace ConfigUtils {
	std::string GetConfigValue(std::string name)
	{
		std::map<std::string, std::string> config;
		std::ifstream configfile("server.cfg");
		std::string line;
		while (std::getline(configfile, line))
		{
			if(line.find_last_of("\r") != std::string::npos)
				line.erase(line.find_last_of("\r"));
			if (line.find_last_of("\n") != std::string::npos)
				line.erase(line.find_last_of("\n"));
			std::string _name;
			std::string _value;
			_name = line.substr(0, line.find_first_of(' '));
			_value = line.substr(line.find_first_of(' ')+1, std::string::npos);
			config[_name] = _value;
		}
		if (config.find(name) == config.end())
			return "ERR_NOT_FOUND";
		else
			return config[name];
	}

	void AppendConfig(std::string name, std::string value)
	{
		FILE* config = fopen("server.cfg", "a");
		char* line = new char[name.length() + value.length() + 10];
		line[0] = 0;
		sprintf(line, "%s %s%s", name.c_str(), value.c_str(), RN);
		fputs(line, config);
		fclose(config);
		delete line;
	}

	bool GetBool(std::string value)
	{
		if (value == "true" || value == "1" || value == "yes")
			return true;
		return false;
	}

	int GetInt(std::string value)
	{
		if (value.empty())
			return 0;
		return std::stoi(value);
	}
}