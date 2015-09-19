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

#ifdef WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <squirrel.h>
#include <SQImports.h>
#include <VCMP.h>
#include "RCON.h"

#ifdef _WIN32
#define RN "\r\n"
#else
#define RN "\n"
#endif

extern PluginFuncs* VCMP_PF;
extern RCON* rcon;

extern HSQUIRRELVM sqvm;
extern HSQAPI sqapi;

extern "C"
{
	EXPORT unsigned int VcmpPluginInit(PluginFuncs* pluginFuncs, PluginCallbacks* pluginCalls, PluginInfo* pluginInfo);
}

int OnInternalCommand(unsigned int uCmdType, const char* pszText);
void OnShutdown();
void OnSquirrelScriptLoad();

namespace ConfigUtils
{
	std::string GetConfigValue(std::string name);
	void AppendConfig(std::string name, std::string value);
	bool GetBool(std::string value);
	int GetInt(std::string value);
};

#ifdef _WIN32
typedef int socklen_t;
#endif

// For exceptions
struct exp : std::exception
{
	char* excepstr;
	const char* what() const noexcept { return excepstr; }
};

#define THROWEXP(exceptionstring) \
{ \
struct exp _exp; \
_exp.excepstr = new char[strlen(exceptionstring)+1]; \
throw _exp; \
}

#endif