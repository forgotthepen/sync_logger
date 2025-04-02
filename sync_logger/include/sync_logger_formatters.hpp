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

#pragma once

#include "sync_logger.hpp"
#include <chrono>


namespace syl {
    struct fmter_type_prefix {
        static void format(syl::logger::entry &entry);
    };

    struct fmter_timestamp {
        static void format(syl::logger::entry &entry);
    };

    struct fmter_con_color {
        // https://stackoverflow.com/a/17469726
        // https://en.wikipedia.org/wiki/ANSI_escape_code
        enum class ConColor {
            RESET = 0,

            FG_BOLD = 1,
            FG_FAINT = 2,

            FG_BLACK = 30,
            FG_RED = 31,
            FG_GREEN = 32,
            FG_YELLOW = 33,
            FG_BLUE = 34,
            FG_MAGENTA = 35,
            FG_DEFAULT = 39,
            FG_BRIGHT_WHITE = 97,

            BG_BLACK = 40,
            BG_RED = 41,
            BG_GREEN = 42,
            BG_YELLOW = 43,
            BG_BLUE = 44,
            BG_MAGENTA = 45,
            BG_DEFAULT = 49,
            BG_BRIGHT_WHITE = 107,
        };
        
        static void format(syl::logger::entry &entry);
    };
}
