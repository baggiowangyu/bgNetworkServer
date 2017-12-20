#include "stdafx.h"
#include "bgCommand.h"

#include "json/json.h"
#include <ObjBase.h>

std::string GenerateGUID()
{
	// 生成一个GUID
	char buffer[128] = {0};  
	GUID guid;  

	if (CoCreateGuid(&guid))  
	{  
		fprintf(stderr, "create guid error\n");  
		return "";  
	}  
	_snprintf(buffer, sizeof(buffer),  
		"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",  
		guid.Data1, guid.Data2, guid.Data3,  
		guid.Data4[0], guid.Data4[1], guid.Data4[2],  
		guid.Data4[3], guid.Data4[4], guid.Data4[5],  
		guid.Data4[6], guid.Data4[7]);  

	return buffer;
}

std::string bgCommand::GeneratePTZCommand(const char *subcmd, const char *manufacturer, const char *cmdval, const char *state, int speed)
{
	std::string id = GenerateGUID();

	// 这里构建json控制命令
	Json::Value arr;
	arr["id"] = Json::Value(id);
	arr["commandtype"] = Json::Value("PTZControl");
	arr["subcmd"] = Json::Value(subcmd);
	arr["manufacturer"] = Json::Value(manufacturer);
	arr["value"] = Json::Value(cmdval);
	arr["state"] = Json::Value(state);
	arr["speed"] = Json::Value(speed);

	Json::StyledWriter sw;
	std::string data = sw.write(arr);
	return data;
}

std::string bgCommand::GenerateStreamCommand(const char *subcmd, const char *manufacturer, const char *cmdval, const char *state)
{
	std::string id = GenerateGUID();

	// 这里构建json控制命令
	Json::Value arr;
	arr["id"] = Json::Value(id);
	arr["commandtype"] = Json::Value("stream");
	arr["subcmd"] = Json::Value(subcmd);
	arr["manufacturer"] = Json::Value(manufacturer);
	arr["value"] = Json::Value(cmdval);
	arr["state"] = Json::Value(state);
	
	Json::StyledWriter sw;
	std::string data = sw.write(arr);
	return data;
}