# Synchronized (thread-safe) logger
A thread-safe logger with an easy message formatting syntax.  
Example
```c++
#include "sync_logger.hpp"
#include <thread>
#include <chrono>

int main() {
    syl::logger::init();

    syl::logger lg{};
    lg.postf(syl::logger::type::info, "wow {} a {} formatted msg", "this is", 1337);
    // "wow this is a 1337 formatted msg"

    std::this_thread::sleep_for(std::chrono::seconds(1));

    syl::logger::deinit();

    return 0;
}
```

The caller (main) thread will not wait for the message to be formatted and printed, all messages are queued and a background thread will process these messages.  
That way the caller(s) can post as many messages as needed without worrying about performance.  
In the above example we waited for ~1 second to allow the background thread to process the message.  
