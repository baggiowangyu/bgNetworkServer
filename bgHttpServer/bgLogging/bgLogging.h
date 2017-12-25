#ifndef _BG_LOGGING_H_
#define _BG_LOGGING_H_

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
	int OnInit(const char *propertise_path);
	void OnClose();

public:
	void WriteLog(const char *log_info, int log_level = LOG_INFO);
	void WriteLog(const wchar_t *log_info, int log_level = LOG_INFO);
};

extern bgLogging logger;
#define LOG_INIT(propertise) logger.OnInit(propertise)
#define LOG(level, info) logger.WriteLog(level, info)

#endif//_BG_LOGGING_H_
