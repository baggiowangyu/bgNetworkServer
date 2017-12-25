#include "bgLogging.h"

#include "log4cxx/logger.h"
#include "log4cxx/propertyconfigurator.h"

bgLogging logger;
log4cxx::LoggerPtr logger_;

bgLogging::bgLogging()
{

}

bgLogging::~bgLogging()
{

}

int bgLogging::OnInit(const char *propertise_path)
{
	log4cxx::PropertyConfigurator::configure(propertise_path);
	logger_ = log4cxx::Logger::getRootLogger();
	return 0;
}

void bgLogging::OnClose()
{
	// 
}

void bgLogging::WriteLog(const char *log_info, int log_level /* = LOG_INFO */)
{
	// 
	switch (log_level)
	{
	case LOG_DEBUG:
		LOG4CXX_DEBUG(logger_, log_info);
		break;
	case LOG_INFO:
		LOG4CXX_INFO(logger_, log_info);
		break;
	case LOG_WARN:
		LOG4CXX_WARN(logger_, log_info);
		break;
	case LOG_ERROR:
		LOG4CXX_ERROR(logger_, log_info);
		break;
	case LOG_FATAL:
		LOG4CXX_FATAL(logger_, log_info);
		break;
	}

	return ;
}

void bgLogging::WriteLog(const wchar_t *log_info, int log_level /* = LOG_INFO */)
{
	switch (log_level)
	{
	case LOG_DEBUG:
		LOG4CXX_DEBUG(logger_, log_info);
		break;
	case LOG_INFO:
		LOG4CXX_INFO(logger_, log_info);
		break;
	case LOG_WARN:
		LOG4CXX_WARN(logger_, log_info);
		break;
	case LOG_ERROR:
		LOG4CXX_ERROR(logger_, log_info);
		break;
	case LOG_FATAL:
		LOG4CXX_FATAL(logger_, log_info);
		break;
	}

	return ;
}