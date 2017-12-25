#ifndef _BG_LOGGING_H_
#define _BG_LOGGING_H_

#include <string>

#define LOG_DEBUG	0x0001
#define LOG_INFO	0x0002
#define LOG_WARN	0x0004
#define LOG_ERROR	0x0008
#define LOG_FATAL	0x0010

class bgLogging
{
public:
	bgLogging();
	~bgLogging();

public:
	int OnInit(const std::string propertise_path);
	int OnInit(const std::wstring propertise_path);
	void OnClose();

public:
	void WriteLog(const char *log_info, int log_level = LOG_INFO);
	void WriteLog(const wchar_t *log_info, int log_level = LOG_INFO);
};

extern bgLogging logger;
#define BG_LOG_INIT(propertise) logger.OnInit(propertise)
#define BG_LOG(info, level) logger.WriteLog(info, level)
#define BG_LOG_DEBUG(info) logger.WriteLog(info, LOG_DEBUG)
#define BG_LOG_INFO(info) logger.WriteLog(info, LOG_INFO)
#define BG_LOG_WARN(info) logger.WriteLog(info, LOG_WARN)
#define BG_LOG_ERROR(info) logger.WriteLog(info, LOG_ERROR)
#define BG_LOG_FATAL(info) logger.WriteLog(info, LOG_FATAL)

#endif//_BG_LOGGING_H_
