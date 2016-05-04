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

#ifndef __REVENTS_H
#define __REVENTS_H

#include "main.h"

namespace Events
{
	void RegisterEvents(PluginCallbacks* vccb);
	void OnPlayerConnect(int32_t playerid);
	void OnPlayerDisconnect(int32_t playerid, vcmpDisconnectReason nReason);
	void OnPlayerSpawn(int32_t playerid);
	void OnPlayerDeath(int32_t playerid, int32_t killerid, int32_t reason, vcmpBodyPart bodypart);
	uint8_t OnMessage(int32_t playerid, const char* message);
	uint8_t OnCommand(int32_t playerid, const char* message);
}

#endif
