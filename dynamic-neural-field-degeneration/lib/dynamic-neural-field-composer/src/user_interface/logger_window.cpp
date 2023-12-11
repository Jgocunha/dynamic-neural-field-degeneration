

#include "user_interface/logger_window.h"


namespace dnf_composer
{
	namespace user_interface
	{
        Logger LoggerWindow::logger;

		LoggerWindow::LoggerWindow()
		{
			logger.autoScroll = true;
			clean();
		}

		void LoggerWindow::render()
		{
            draw();
		}

		void LoggerWindow::clean()
		{
			logger.buffer.clear();
			logger.lineOffsets.clear();
            logger.lineOffsets.push_back(0);
		}

        LogLevel LoggerWindow::getLogLevelFromLine(const char* line_start)
        {
            const char* infoTag = "[INFO]";
            const char* warningTag = "[WARNING]";
            const char* errorTag = "[ERROR]";

            // Check if the line contains the INFO tag
            if (std::strncmp(line_start, infoTag, std::strlen(infoTag)) == 0)
                return LogLevel::_INFO;

            // Check if the line contains the WARNING tag
            if (std::strncmp(line_start, warningTag, std::strlen(warningTag)) == 0)
                return LogLevel::_WARNING;

            // Check if the line contains the ERROR tag
            if (std::strncmp(line_start, errorTag, std::strlen(errorTag)) == 0)
                return LogLevel::_ERROR;

            // Default to INFO if no specific tag is found
            return LogLevel::_INFO;
        }

        void LoggerWindow::drawLog()
		{

            const char* buf = logger.buffer.begin();
            const char* buf_end = logger.buffer.end();

            if (logger.filter.IsActive())
            {
                // In this example we don't use the clipper when Filter is enabled.
                // This is because we don't have random access to the result of our filter.
                // A real application processing logs with ten of thousands of entries may want to store the result of
                // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
                for (int line_no = 0; line_no < logger.lineOffsets.Size; line_no++)
                {
                    const char* line_start = buf + logger.lineOffsets[line_no];
                    const char* line_end = (line_no + 1 < logger.lineOffsets.Size) ? (buf + logger.lineOffsets[line_no + 1] - 1) : buf_end;

                    ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // Default to white

                    switch (getLogLevelFromLine(line_start))
                    {
                    case LogLevel::_INFO:
                        textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // White
                        break;
                    case LogLevel::_WARNING:
                        textColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow
                        break;
                    case LogLevel::_ERROR:
                        textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);  // Red
                        break;
                    }

                    if (logger.filter.PassFilter(line_start, line_end))
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                        ImGui::TextUnformatted(line_start, line_end);
                        ImGui::PopStyleColor();
                    }
                }
            }
            else
            {
                // The simplest and easy way to display the entire buffer:
                //   ImGui::TextUnformatted(buf_begin, buf_end);
                // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
                // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
                // within the visible area.
                // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
                // on your side is recommended. Using ImGuiListClipper requires
                // - A) random access into your data
                // - B) items all being the  same height,
                // both of which we can handle since we have an array pointing to the beginning of each line of text.
                // When using the filter (in the block of code above) we don't have random access into the data to display
                // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
                // it possible (and would be recommended if you want to search through tens of thousands of entries).
                ImGuiListClipper clipper;
                clipper.Begin(logger.lineOffsets.Size);
                while (clipper.Step())
                {
                    for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                    {
                        const char* line_start = buf + logger.lineOffsets[line_no];
                        const char* line_end = (line_no + 1 < logger.lineOffsets.Size) ? (buf + logger.lineOffsets[line_no + 1] - 1) : buf_end;

                        ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // Default to white

                        switch (getLogLevelFromLine(line_start))
                        {
                        case LogLevel::_INFO:
                            textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // White
                            break;
                        case LogLevel::_WARNING:
                            textColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);  // Yellow
                            break;
                        case LogLevel::_ERROR:
                            textColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);  // Red
                            break;
                        }
                        ImGui::PushStyleColor(ImGuiCol_Text, textColor);
                        ImGui::TextUnformatted(line_start, line_end);
                        ImGui::PopStyleColor();
                    }
                }
                clipper.End();
            }
		}

		void LoggerWindow::draw()
		{
            if (!ImGui::Begin("Log window"))
            {
                ImGui::End();
                return;
            }

            // Options menu
            if (ImGui::BeginPopup("Options"))
            {
                ImGui::Checkbox("Auto-scroll", &logger.autoScroll);
                ImGui::EndPopup();
            }

            // Main window
            if (ImGui::Button("Options"))
                ImGui::OpenPopup("Options");
            ImGui::SameLine();
            const bool clear = ImGui::Button("Clear");
            ImGui::SameLine();
            const bool copy = ImGui::Button("Copy");
            ImGui::SameLine();
            logger.filter.Draw("Filter", -100.0f);

            ImGui::Separator();

            if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
            {
                if (clear)
                    clean();
                if (copy)
                    ImGui::LogToClipboard();

                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
                drawLog();
                ImGui::PopStyleVar();

                // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
                // Using a scrollbar or mouse-wheel will take away from the bottom edge.
                if (logger.autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                    ImGui::SetScrollHereY(1.0f);
            }
            ImGui::EndChild();
            ImGui::End();
		}

        void LoggerWindow::addLog(LogLevel level, const char* message, ...)
        {
            int old_size = logger.buffer.size();
            va_list args;
            va_start(args, message);

            // Get the current time
            const auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);

            std::tm timeinfo;
#if defined(_WIN32)
            if (localtime_s(&timeinfo, &time_t_now) != 0)
            {
                throw std::runtime_error("Failed to get local time");
            }
#else
            if (localtime_r(&time_t_now, &timeinfo) == nullptr)
            {
                throw std::runtime_error("Failed to get local time");
            }
#endif
            // Convert the time to a string
            char time_str[20];  // Adjust the buffer size as needed
            if (strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &timeinfo) == 0)
            {
                throw std::runtime_error("Failed to format time as string");
            }

            // Append log level and message
            switch (level)
            {
            case LogLevel::_INFO:
                logger.buffer.appendf("[INFO] ");
                break;
            case LogLevel::_WARNING:
                logger.buffer.appendf("[WARNING] ");
                break;
            case LogLevel::_ERROR:
                logger.buffer.appendf("[ERROR] ");
                break;
                // Add more cases as needed
            }

            // Append the log message
            logger.buffer.appendfv(message, args);

            // Append the timestamp at the end
            logger.buffer.appendf(" - [%s]", time_str);

            logger.buffer.append("\n");
            va_end(args);

            // Update lineOffsets
            for (const int new_size = logger.buffer.size(); old_size < new_size; old_size++)
                if (logger.buffer[old_size] == '\n')
                    logger.lineOffsets.push_back(old_size + 1);
        }

	}
}
