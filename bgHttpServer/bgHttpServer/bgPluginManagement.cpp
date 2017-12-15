#include "stdafx.h"

#include "bgPluginManagement.h"


bgPluginManagement::bgPluginManagement()
{

}

bgPluginManagement::~bgPluginManagement()
{

}

int bgPluginManagement::InstallPlugin(std::string plugin_name, bgHttpBusinessPlugins* plugin, std::string path)
{
	int errCode = 0;

	// 加载插件
	HMODULE hMod = LoadLibraryA(path.c_str());
	if (hMod == nullptr)
	{
		errCode = GetLastError();
		return errCode;
	}

	// 找到创建插件实例的接口
	_CreateObject ptr_CreateObject = (_CreateObject)GetProcAddress(hMod, "CreateObject");
	if (ptr_CreateObject == nullptr)
	{
		errCode = ERROR_NOT_FOUND;
		FreeLibrary(hMod);
		return errCode;
	}

	_DestroyObject ptr_DestroyObject = (_DestroyObject)GetProcAddress(hMod, "DestroyObject");
	if (ptr_DestroyObject == nullptr)
	{
		errCode = ERROR_NOT_FOUND;
		FreeLibrary(hMod);
		return errCode;
	}

	// 创建实例
	bgHttpBusinessPlugins* plugin = ptr_CreateObject();
	if (plugin == nullptr)
	{
		errCode = ERROR_NOT_FOUND;
		FreeLibrary(hMod);
		return errCode;
	}

	PLUGIN_INFO plugin_info;
	plugin_info.plugin_object_ = plugin;
	plugin_info.destructor_ = ptr_DestroyObject;

	plugins_.insert(std::pair<std::string, PLUGIN_INFO>(plugin_name, plugin_info));

	return errCode;
}

int bgPluginManagement::RemovePlugin(std::string plugin_name)
{
	std::map<std::string, PLUGIN_INFO>::iterator iter = plugins_.find(plugin_name);

	// 先释放掉对象
	_DestroyObject ptr_DestroyObject = iter->second.destructor_;
	ptr_DestroyObject(iter->second.plugin_object_);

	// 释放库
	FreeLibrary(iter->second.module_handle_);

	// 删除元素
	plugins_.erase(plugin_name);
	return 0;
}

void bgPluginManagement::CleanupAllPlugins()
{
	plugins_.clear();
}

bgHttpBusinessPlugins* bgPluginManagement::FindPlugin(std::string plugin_name)
{
	std::map<std::string, PLUGIN_INFO>::iterator iter = plugins_.find(plugin_name);
	if (iter == plugins_.end())
		return nullptr;

	return iter->second.plugin_object_;
}

bgHttpBusinessPlugins* bgPluginManagement::GetFirstPlugin()
{
	plugin_iter_ = plugins_.begin();
	return plugin_iter_->second.plugin_object_;
}

bgHttpBusinessPlugins* bgPluginManagement::GetNextPlugin()
{
	++plugin_iter_;
	if (plugin_iter_ == plugins_.end())
		return nullptr;

	return plugin_iter_->second.plugin_object_;
}