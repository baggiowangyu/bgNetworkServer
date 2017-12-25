#include "stdafx.h"

#include "..\bgLogging\extern_logging.h"

#include "bgPluginManagement.h"


bgPluginManagement::bgPluginManagement()
{

}

bgPluginManagement::~bgPluginManagement()
{

}

int bgPluginManagement::InstallPlugin(std::string plugin_name,std::string path, std::string cfg_path)
{
	int errCode = 0;
	char msg[4096] = {0};

	// 加载插件
	HMODULE hMod = LoadLibraryA(path.c_str());
	if (hMod == nullptr)
	{
		errCode = GetLastError();
		sprintf_s(msg, 4096, "Load library %s failed... errcode : %d", path.c_str(), errCode);
		LOG4CXX_WARN(rootLogger, msg);
		return errCode;
	}

	// 找到创建插件实例的接口
	_CreateObject ptr_CreateObject = (_CreateObject)GetProcAddress(hMod, "CreateObject");
	if (ptr_CreateObject == nullptr)
	{
		errCode = ERROR_NOT_FOUND;
		FreeLibrary(hMod);
		sprintf_s(msg, 4096, "Not found interface \"CreateObject\" in module %s", path.c_str());
		LOG4CXX_WARN(rootLogger, msg);
		return errCode;
	}

	_DestroyObject ptr_DestroyObject = (_DestroyObject)GetProcAddress(hMod, "DestroyObject");
	if (ptr_DestroyObject == nullptr)
	{
		errCode = ERROR_NOT_FOUND;
		FreeLibrary(hMod);
		sprintf_s(msg, 4096, "Not found interface \"DestroyObject\" in module %s", path.c_str());
		LOG4CXX_WARN(rootLogger, msg);
		return errCode;
	}

	// 创建实例
	bgHttpBusinessPlugins *plugin = ptr_CreateObject();
	if (plugin == nullptr)
	{
		errCode = ERROR_NOT_FOUND;
		FreeLibrary(hMod);
		sprintf_s(msg, 4096, "Create plugin \"%s\" instance failed", plugin_name.c_str());
		LOG4CXX_WARN(rootLogger, msg);
		return errCode;
	}

	// 直接执行插件初始化，如果初始化失败，也说明插件加载失败了
	errCode = plugin->Init(cfg_path.c_str());
	if (errCode != 0)
	{
		ptr_DestroyObject(&plugin);
		//FreeLibrary(hMod);
		sprintf_s(msg, 4096, "Initialize plugin \"%s\" failed", plugin_name.c_str());
		LOG4CXX_WARN(rootLogger, msg);
		return errCode;
	}

	PLUGIN_INFO plugin_info;
	plugin_info.plugin_object_ = plugin;
	plugin_info.destructor_ = ptr_DestroyObject;
	plugin_info.module_handle_ = hMod;

	plugins_.insert(std::pair<std::string, PLUGIN_INFO>(plugin_name, plugin_info));

	return errCode;
}

int bgPluginManagement::RemovePlugin(std::string plugin_name)
{
	std::map<std::string, PLUGIN_INFO>::iterator iter = plugins_.find(plugin_name);

	// 先释放掉对象
	_DestroyObject ptr_DestroyObject = iter->second.destructor_;
	ptr_DestroyObject(&iter->second.plugin_object_);

	// 释放库
	//FreeLibrary(iter->second.module_handle_);

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