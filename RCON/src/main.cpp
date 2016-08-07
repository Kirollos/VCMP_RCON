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
#include "events.h"
#include "SquirrelFunctions.h"

PluginFuncs* VCMP_PF = nullptr;
RCON* rcon = nullptr;

HSQUIRRELVM sqvm;
HSQAPI sqapi;

std::map<std::string, vcmpServerOption> vcmp_toggleables;

extern "C"
{
	EXPORT unsigned int VcmpPluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo)
	{
#ifdef _DEBUG
		Sleep(5000); // In need to attach debugger sometimes
#endif
		strcpy(pluginInfo->name, "RCON");
		pluginInfo->pluginVersion = VERSION;
		pluginInfo->apiMajorVersion = 2;
		pluginInfo->apiMinorVersion = 0;
		VCMP_PF = pluginFuncs;
		char* version = new char[20];
		GetRVersion(version);
		VCMP_PF->LogMessage("RCON plugin v%s (c) Kirollos 2015-2016", version);
		VCMP_PF->LogMessage("Initializing RCON...");
		delete version;
		
		std::string enabled = ConfigUtils::GetConfigValue("rcon_enabled"),
			port = ConfigUtils::GetConfigValue("rcon_port"),
			bindip = ConfigUtils::GetConfigValue("rcon_bindip"),
			password = ConfigUtils::GetConfigValue("rcon_password"),
			eevents = ConfigUtils::GetConfigValue("rcon_enableevents");

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
		if (eevents == "ERR_NOT_FOUND")
		{
			ConfigUtils::AppendConfig("rcon_enableevents", "true");
			eevents = "true";
		}

		if (!ConfigUtils::GetBool(enabled))
		{
			VCMP_PF->LogMessage("[RCON]: Plugin is disabled.");
			return 1;
		}

		if (ConfigUtils::GetInt(port) == 0)
			port = "1337";
		if (bindip.empty())
			bindip = "0.0.0.0";
		if (password.empty())
			password = "plschange";
		
		pluginCalls->OnServerShutdown = OnShutdown;
		pluginCalls->OnPluginCommand = OnInternalCommand;
		try {
			rcon = new RCON(ConfigUtils::GetInt(port), bindip, password);
		}
		catch (std::exception& e)
		{
			VCMP_PF->LogMessage("[RCON]: Failed to initialize the plugin, given error: %s", e.what());
			return 0;
		}
		if(ConfigUtils::GetBool(eevents))
			Events::RegisterEvents(pluginCalls);
		InitializeToggleables(&vcmp_toggleables, VCMP_PF);
		VCMP_PF->LogMessage("[RCON]: Plugin is enabled.");
		return 1;
	}
}

uint8_t OnInternalCommand(uint32_t uCmdType, const char* pszText)
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
	size_t size;
	int32_t sqID = VCMP_PF->FindPlugin("SQHost2");
	const void** sqExports = VCMP_PF->GetPluginExports(sqID, &size);

	if (sqExports != NULL && size > 0)
	{
		SquirrelImports* sqFuncs = (SquirrelImports*)(*sqExports);
		if (sqFuncs)
		{
			sqvm = *sqFuncs->GetSquirrelVM();
			sqapi = *sqFuncs->GetSquirrelAPI();

			SquirrelFuncs::RegisterFunctions(&sqvm);
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
			if (line.empty())
				continue;
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

void GetRVersion(int* major, int* minor, int* patch, int* spatch)
{
	*major = (VERSION & 0xF000) >> 12,
	*minor = (VERSION & 0xF00) >> 8,
	*patch = (VERSION & 0xF0) >> 4,
	*spatch = (VERSION & 0xF);
}

void GetRVersion(char* ret)
{
	int v_major, v_minor, v_patch, v_spatch;
	GetRVersion(&v_major, &v_minor, &v_patch, &v_spatch);
	ret[0] = '\0';
	sprintf(ret, "%d.%d.%d.%d", v_major, v_minor, v_patch, v_spatch);
}

void InitializeToggleables(std::map<std::string, vcmpServerOption>* togs, PluginFuncs* pf)
{
	(*togs)["syncfpslimiter"] = vcmpServerOption::vcmpServerOptionSyncFrameLimiter;
	(*togs)["fpslimiter"] = vcmpServerOption::vcmpServerOptionFrameLimiter;
	(*togs)["taxiboostjump"] = vcmpServerOption::vcmpServerOptionTaxiBoostJump;
	(*togs)["driveonwater"] = vcmpServerOption::vcmpServerOptionDriveOnWater;
	(*togs)["fastswitch"] = vcmpServerOption::vcmpServerOptionFastSwitch;
	(*togs)["friendlyfire"] = vcmpServerOption::vcmpServerOptionFriendlyFire;
	(*togs)["disabledriveby"] = vcmpServerOption::vcmpServerOptionDisableDriveBy;
	(*togs)["perfecthandling"] = vcmpServerOption::vcmpServerOptionPerfectHandling;
	(*togs)["flyingcars"] = vcmpServerOption::vcmpServerOptionFlyingCars;
	(*togs)["jumpswitch"] = vcmpServerOption::vcmpServerOptionJumpSwitch;
	(*togs)["showmarkers"] = vcmpServerOption::vcmpServerOptionShowMarkers;
	(*togs)["stuntbike"] = vcmpServerOption::vcmpServerOptionStuntBike;
	(*togs)["shootinair"] = vcmpServerOption::vcmpServerOptionShootInAir;
	(*togs)["shownametags"] = vcmpServerOption::vcmpServerOptionShowNameTags;
	(*togs)["joinmessages"] = vcmpServerOption::vcmpServerOptionJoinMessages;
	(*togs)["deathmessages"] = vcmpServerOption::vcmpServerOptionDeathMessages;
	(*togs)["chattags"] = vcmpServerOption::vcmpServerOptionChatTagsEnabled;
}
