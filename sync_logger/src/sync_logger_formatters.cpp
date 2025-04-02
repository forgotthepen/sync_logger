/*
MIT License

Copyright (c) 2024 forgotthepen (https://github.com/forgotthepen/sync_logger)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "sync_logger_formatters.hpp"
#include <ostream>
#include <iomanip>
#include <ctime>
#include <time.h>


namespace syl {
    void fmter_type_prefix::format(logger::entry &entry) {
        const char *prefix{};
        switch (entry.msg_type) {
        case logger::type::info: prefix = "[Info] "; break;
        case logger::type::warn: prefix = "[Warn] "; break;
        case logger::type::error: prefix = "[ERROR] "; break;
        case logger::type::debug: prefix = "[Debug] "; break;
        default: break;
        }
        
        if (prefix) {
            entry.msg = prefix + entry.msg;
        }
    }

    void fmter_timestamp::format(logger::entry &entry) {
        const static auto format_time = [](const std::chrono::system_clock::time_point &timepoint) {
            auto time = std::chrono::system_clock::to_time_t(timepoint);
    
            std::tm local_tm{};
        
#if defined(_WIN32)
            localtime_s(&local_tm, &time);
#else
            localtime_r(&time, &local_tm);
#endif
        
            char time_buffer[30]{};
            std::strftime(time_buffer, sizeof(time_buffer), "%Y/%m/%d %H:%M:%S", &local_tm);
        
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(timepoint.time_since_epoch()) % 1000;
        
            std::ostringstream fmt_time{};
            fmt_time << time_buffer << '.' << std::setfill('0') << std::setw(3) << ms.count();
            return fmt_time.str();
        };

        entry.msg = '[' + format_time(entry.time) + "] " + entry.msg;
    }

    void fmter_con_color::format(logger::entry &entry) {
        const static auto con_color_to_str = [](fmter_con_color::ConColor color) {
            std::ostringstream ss{};
            ss << "\033[" << (int)color << "m";
            return ss.str();
        };

        ConColor bg_color{};
        switch (entry.msg_type) {
        case logger::type::debug: bg_color = ConColor::BG_MAGENTA; break;
        case logger::type::error: bg_color = ConColor::BG_RED; break;
        case logger:: type::info: bg_color = ConColor::BG_BLACK; break;
        case logger::type::warn: bg_color = ConColor::BG_YELLOW; break;
        default: bg_color = ConColor::BG_BLACK; break;
        }
        
        std::ostringstream ss{};
        ss << con_color_to_str(ConColor::FG_BOLD) << con_color_to_str(ConColor::BG_BRIGHT_WHITE) << con_color_to_str(bg_color)
           << entry.msg
           << con_color_to_str(ConColor::RESET);
        
        entry.msg = ss.str();
    }
}
