#ifndef _BG_COMMAND_H_
#define _BG_COMMAND_H_

#include <string>

class bgCommand
{
public:
	static std::string GeneratePTZCommand(const char *subcmd, const char *manufacturer, const char *cmdval, const char *state, int speed);
	static std::string GenerateStreamCommand(const char *subcmd, const char *manufacturer, const char *cmdval, const char *state);
};

#endif
