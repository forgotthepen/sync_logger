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

#include "sync_logger.hpp"
#include <iostream>


namespace syl {
    constexpr const char logger::INDICATOR[];

    bool logger::kill_thread{};
    std::condition_variable logger::event_cv{};
    std::deque<logger::entry> logger::data_q{};
    std::mutex logger::main_lock{};
    std::thread logger::main_thread{};


    void logger::worker_proc() {
        while (!kill_thread) {
            {
                std::unique_lock<std::mutex> lock(main_lock);
                event_cv.wait(lock, [] { return kill_thread || !data_q.empty(); });
            }

            while (!kill_thread && !data_q.empty()) {
                main_lock.lock();
                
                entry ref = std::move(data_q.front());
                data_q.pop_front();

                main_lock.unlock();

                for (auto &fmter : ref.instance->fmter_fns) {
                    fmter(ref);
                }
                
                auto &out_stream = type::error == ref.msg_type ? std::cerr : std::cout;
                out_stream << ref.msg << '\n';
            }
        }
    }


    void logger::post(const std::string &msg, type msg_type, place places) {
        entry ent{};
        ent.instance = this;
        ent.msg_type = msg_type;
        ent.msg_places = places;
        ent.msg = msg;
    
        {
            std::lock_guard<std::mutex> lock(main_lock);
            data_q.emplace_back(std::move(ent));
        }
    
        event_cv.notify_one();
    }
    
    void logger::init() {
        std::lock_guard<std::mutex> lock(main_lock);
    
        if (main_thread.joinable()) {
            return;
        }
    
        kill_thread = false;
        main_thread = std::thread(&logger::worker_proc);
    }
    
    void logger::deinit() {
        {
            std::lock_guard<std::mutex> lock(main_lock);
    
            if (!main_thread.joinable()) {
                return;
            }
    
            kill_thread = true;
        }
    
        event_cv.notify_one();
        main_thread.join();
        
        {
            std::lock_guard<std::mutex> lock(main_lock);
    
            data_q.clear();
        }
    
    }
}
