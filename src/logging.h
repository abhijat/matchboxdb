#ifndef MATCHBOXDB_LOGGING_H
#define MATCHBOXDB_LOGGING_H

#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

namespace log {

template<typename T>
void unpack_to_string(std::ostream &os, const T &t) {
    os << t;
}

template<typename T, typename... P>
void unpack_to_string(std::ostream &os, const T &t, P&... args) {
    os << t << " ";
    unpack_to_string(os, args...);
}

template<typename... P>
void info(P&... args) {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    std::stringstream ss;
    unpack_to_string(ss, args...);

    std::cout << std::put_time(&tm, "[%c %Z]") << " :: " << ss.str() << "\n";
}

}

#endif //MATCHBOXDB_LOGGING_H
