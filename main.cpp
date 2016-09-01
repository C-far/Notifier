#include "main.h"

void DLL_EXPORT SomeFunction(const LPCSTR sometext)
{
    MessageBoxA(0, sometext, "DLL Message", MB_OK | MB_ICONINFORMATION);
}

extern "C" DLL_EXPORT BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_THREAD_ATTACH:
            break;

        case DLL_THREAD_DETACH:
            break;
    }
    return TRUE;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "teamspeak/public_errors.h"
#include "teamspeak/public_errors_rare.h"
#include "teamspeak/public_definitions.h"
#include "teamspeak/public_rare_definitions.h"
#include "teamspeak/clientlib_publicdefinitions.h"
#include "ts3_functions.h"
#include "plugin.h"

static struct TS3Functions ts3Functions;

#define PLUGIN_API_VERSION 20

static char* pluginID = NULL;

#ifdef _WIN32

	static int wcharToUtf8(const wchar_t* str, char** result)
	{
		int outlen = WideCharToMultiByte(CP_UTF8, 0, str, -1, 0, 0, 0, 0);

		*result = (char*)malloc(outlen);

		if(WideCharToMultiByte(CP_UTF8, 0, str, -1, *result, outlen, 0, 0) == 0)
		{
			*result = NULL;
			return -1;
		}

		return 0;
	}

#endif

void ts3plugin_setFunctionPointers(const struct TS3Functions funcs)
{
	ts3Functions = funcs;
}

int ts3plugin_init()
{
	return 0;
}

void ts3plugin_shutdown()
{
	if(pluginID)
	{
		free(pluginID);
		pluginID = NULL;
	}
}

void ts3plugin_freeMemory(void* data)
{
	free(data);
}

int ts3plugin_requestAutoload()
{
	return 1;
}

const char* ts3plugin_name()
{
	#ifdef _WIN32
		static char*
			result = NULL;

		if(!result)
		{
			const wchar_t*
				name = L"Notifier";

			if(wcharToUtf8(name, &result) == -1)
			{
				result = "Notifier";
			}
		}
		return result;
	#else
		return "Notifier";
	#endif
}

const char* ts3plugin_version()
{
	return "0.3";
}

int ts3plugin_apiVersion()
{
	return PLUGIN_API_VERSION;
}

const char* ts3plugin_author()
{
	return "C_far";
}

const char* ts3plugin_description()
{
	return "Notification sonore pour le staff de FunCraft lorsqu'un joueur rejoint une demande d'aide.";
}

void ts3plugin_onClientMoveEvent(uint64 serverConnectionHandlerID, anyID clientID, uint64 oldChannelID, uint64 newChannelID, int visibility, const char* moveMessage)
{
    uint64 parentChannelID;

    ts3Functions.getParentChannelOfChannel(serverConnectionHandlerID, newChannelID, &parentChannelID);

	if(parentChannelID == (uint64)25)
	{
	    uint64 parentOldChannelID;

        ts3Functions.getParentChannelOfChannel(serverConnectionHandlerID, oldChannelID, &parentOldChannelID);

	    if(parentOldChannelID == parentChannelID) return;

	    anyID *clientList;
        ts3Functions.getChannelClientList(serverConnectionHandlerID, newChannelID, &clientList);

        int nbr = 0;

        while(clientList[nbr]) nbr++;

        if(nbr == 1) ts3Functions.playWaveFile(serverConnectionHandlerID, "sound/default/notifier.wav");
	}

	return;
}
