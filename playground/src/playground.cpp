#include <iostream>
#include "sync_logger.hpp"
#include "sync_logger_formatters.hpp"


struct mc {
    operator std::string() {
        return "|mc instance|";
    }
};

struct soo {};
std::ostream& operator<<(std::ostream &os, const soo&) {
    os << "|soo instance|";
    return os;
}

struct daa {};
namespace std {
    std::string to_string(const daa&) {
        return "|daa instance|";
    }
}


int main() {
    syl::logger::init();

    syl::logger lg{};

    lg.add_formatter<syl::fmter_type_prefix>()
      .add_formatter<syl::fmter_timestamp>()
      .add_formatter<syl::fmter_con_color>()
    ;

    lg.post("hello from logger", syl::logger::type::info);
    lg.postf(syl::logger::type::info, "wow {} a {} formatted msg", "this is", 1337);


    while (true) {
        std::string x{};
        std::cin >> x;
        if (x == "x" || x == "X") {
            break;
        }
        lg.postf(syl::logger::type::debug, "your message: '{}' {} {} {} {} {} {} {} {} {}",
            x, lg, mc{}, soo{}, daa{}, 4,5.1,6.2f,7UL,9L);
        lg.postf(syl::logger::type::error, "your message: '{}'", x);
        lg.postf(syl::logger::type::info, "your message: '{}'", x);
        lg.postf(syl::logger::type::warn, "your message: '{}'", x);
        
    }

    syl::logger::deinit();
    
    std::cout << "done" << std::endl;

    return 0;
}
