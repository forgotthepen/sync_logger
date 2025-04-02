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

#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <deque>
#include <chrono>
#include <utility>
#include <string>
#include <sstream>
#include <cstdint>
#include <functional>
#include <type_traits>


namespace syl {
    class logger {
    public:
        enum class type {
            info,
            warn,
            error,
            debug,
        };
        
        enum place : uint32_t {
            console = 1 << 0,
            file    = 1 << 1,
        };

        struct entry {
            class logger* instance{};
            logger::type msg_type{};
            logger::place msg_places{};
            std::string msg{};
            std::chrono::system_clock::time_point time = std::chrono::system_clock::now();
        };

        constexpr static const place all_places = (place)(place::console | place::file);

    private:
        using fmter_fn_t = void (*)(entry&);
        constexpr static const char INDICATOR[] = "{}";


        template<typename T>
        class is_string_castable {
            template<typename T2, typename = void>
            struct test : std::false_type {};
            
            template<typename T2>
            struct test<T2,
                decltype(static_cast<std::string>(std::declval<T2>()), void())
            > : std::true_type {};
            
        public:
            constexpr static bool value = test<T>::value;
        };
        
        template<typename T>
        class can_use_ostream {
            template<typename T2, typename = void>
            struct test : std::false_type {};
            
            template<typename T2>
            struct test<T2,
                decltype(std::declval<std::ostream&>() << std::declval<T2>(), void())
            > : std::true_type {};
            
        public:
            constexpr static bool value = test<T>::value;
        };
        
        template<typename T>
        class has_std_to_string {
            template<typename T2, typename = void>
            struct test : std::false_type {};
            
            template<typename T2>
            struct test<T2,
                decltype(static_cast<std::string>(std::to_string(std::declval<T2>())), void())
            > : std::true_type {};
            
        public:
            constexpr static bool value = test<T>::value;
        };
        
        
        struct string_converter {
            template <typename T>
            static typename std::enable_if<
                is_string_castable<T>::value,
            std::string>::type convert(T&& obj) {
                return static_cast<std::string>(obj);
            }
            
            template <typename T>
            static typename std::enable_if<
                !is_string_castable<T>::value
                && can_use_ostream<T>::value,
            std::string>::type convert(T&& obj) {
                std::ostringstream ss{};
                ss << std::forward<T>(obj);
                return ss.str();
            }
            
            template <typename T>
            static typename std::enable_if<
                !is_string_castable<T>::value
                && !can_use_ostream<T>::value
                && has_std_to_string<T>::value,
            std::string>::type convert(T&& obj) {
                return std::to_string(std::forward<T>(obj));
            }
            
            template <typename T>
            static std::string convert(...) {
                std::ostringstream ss{};
                ss << "<object: '" << typeid(T).name() << "'>";
                return ss.str();
            }
        };
        
        
        template<typename T>
        class is_valid_fmter {
            template<typename T2, typename = void>
            struct test : std::false_type {};
            
            template<typename T2>
            struct test<T2, typename std::enable_if<
                std::is_same<fmter_fn_t, decltype(&T2::format)>::value
            >::type> : std::true_type {};

        public:
            constexpr static bool value = test<T>::value;
        };

        
        std::vector<fmter_fn_t> fmter_fns{};

        static bool kill_thread;
        static std::condition_variable event_cv;
        static std::deque<entry> data_q;
        static std::mutex main_lock;
        static std::thread main_thread;

        static void worker_proc();

    public:
        static void init();
        static void deinit();

        void post(const std::string &msg, type msg_type = type::info, place places = all_places);

        template<typename ...Args>
        inline void postf(type msg_type, const std::string &fmt, Args&& ...args) {
            std::vector<std::function<std::string ()>> args_strs {
                [&args] { return string_converter::convert<Args>(std::forward<Args>(args)); } ...
            };
            
            std::ostringstream ss{};
            size_t arg_idx = 0;
            size_t last_pos = 0;
            while (true) {
                size_t pos = fmt.find(INDICATOR, last_pos);
                if (std::string::npos == pos) {
                    ss << fmt.substr(last_pos);
                    break;
                }

                ss << fmt.substr(last_pos, pos - last_pos);
                last_pos = pos + sizeof(INDICATOR) - 1;

                if (arg_idx < args_strs.size()) {
                    ss << args_strs[arg_idx]();
                    ++arg_idx;
                }
            }
            
            post(ss.str(), msg_type, all_places);
        }

        template<typename T>
        inline typename std::enable_if<is_valid_fmter<T>::value, logger&>::type add_formatter() {
            fmter_fns.push_back(&T::format);
            return *this;
        }
    };
}
