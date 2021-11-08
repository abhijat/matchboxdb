#ifndef MATCHBOXDB_LOGGING_H
#define MATCHBOXDB_LOGGING_H

#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

namespace log {


template<typename T>
void unpack_to_string(std::ostream &os, T &&t) {
    os << t;
}

template<typename T, typename... P>
void unpack_to_string(std::ostream &os, T &&t, P&&... args) {
    os << t << " ";
    unpack_to_string(os, std::forward<P>(args)...);
}

template<typename... P>
void info(P&&... args) {
    static std::mutex m{};

    std::lock_guard<std::mutex> guard{m};

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    std::stringstream ss;
    unpack_to_string(ss, std::forward<P>(args)...);

    std::cout << std::put_time(&tm, "[%c %Z]") << " :: " << ss.str() << "\n";
}

}

#endif //MATCHBOXDB_LOGGING_H
