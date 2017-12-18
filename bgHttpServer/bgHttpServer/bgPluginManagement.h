#ifndef _BG_PLUGIN_MANAGEMENT_H_
#define _BG_PLUGIN_MANAGEMENT_H_

#include "bgHttpBusinessPlugins.h"
#include <string>
#include <map>
#include <Windows.h>

extern "C" typedef bgHttpBusinessPlugins* (__stdcall * _CreateObject)();
extern "C" typedef void (__stdcall * _DestroyObject)(bgHttpBusinessPlugins* plugin);

typedef struct _PLUGIN_INFO_
{
	bgHttpBusinessPlugins* plugin_object_;
	_DestroyObject destructor_;
	HMODULE module_handle_;

} PLUGIN_INFO, *PPLUGIN_INFO;

class bgPluginManagement
{
public:
	bgPluginManagement();
	~bgPluginManagement();

public:
	int InstallPlugin(std::string plugin_name, std::string path);
	int RemovePlugin(std::string plugin_name);
	void CleanupAllPlugins();

	bgHttpBusinessPlugins* FindPlugin(std::string plugin_name);
	bgHttpBusinessPlugins* GetFirstPlugin();
	bgHttpBusinessPlugins* GetNextPlugin();

private:
	std::map<std::string, PLUGIN_INFO> plugins_;
	std::map<std::string, PLUGIN_INFO>::iterator plugin_iter_;
};

#endif//_BG_PLUGIN_MANAGEMENT_H_
