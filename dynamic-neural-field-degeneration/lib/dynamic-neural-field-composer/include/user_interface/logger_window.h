#pragma once

#include <string>
#include <chrono>

#include "./user_interface/user_interface_window.h"

namespace dnf_composer
{
	namespace user_interface
	{
		enum class LogLevel : int
		{
			_INFO,
			_WARNING,
			_ERROR
		};

		struct Logger
		{
			ImGuiTextBuffer     buffer;
			ImGuiTextFilter     filter;
			ImVector<int>       lineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
			bool                autoScroll = true;  // Keep scrolling if already at the bottom.
		};

		class LoggerWindow : public UserInterfaceWindow
		{
		private:
			static Logger logger;
			std::string windowTitle;
		public:
			LoggerWindow();
			static void addLog(LogLevel level, const char* message, ...) IM_FMTARGS(3);

			void render() override;
			~LoggerWindow() override = default;
		private:
            static void clean();
			static void draw();
			static LogLevel getLogLevelFromLine(const char* line_start);
			static void drawLog();
		};
	}
}