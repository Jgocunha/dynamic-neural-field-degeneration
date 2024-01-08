#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>

#include "exceptions/exception.h"
#include "user_interface/logger_window.h"

namespace dnf_composer
{
	enum LogLevel : int
	{
		DEBUG = 0,
		INFO,
		WARNING,
		ERROR_, // ERROR is already defined in windows.h
		FATAL
	};

	enum LogOutputMode : int
	{
		CONSOLE,
		GUI,
		ALL
	};

	class Logger
	{
	private:
		LogLevel logLevel;
		LogOutputMode outputMode;
	public:
		Logger(LogLevel level, LogOutputMode mode = ALL);
		void log(const std::string& message) const;
	private:
		static std::string getLogLevelColorCode(LogLevel level);
		static std::string getLogLevelText(LogLevel level);
		static void log_cmd(const std::string& message);
		static void log_ui(const std::string& message);
	};

	void log(LogLevel level, const std::string& message, LogOutputMode mode = ALL);

	static Logger logger(LogLevel::INFO);
}