#pragma once

#include <string>
#include <iomanip>
#include <sstream>
#include <cmath>

typedef unsigned char byte;
typedef unsigned int uint;

template <class T>
std::string toString(T t, int base, int precision) {
    std::ostringstream s;

    s << std::setbase(base) << std::setprecision(precision) << t;

    return s.str();
}

template <class T>
T fromString(const std::string &str) {
    std::istringstream s(str);

    T t;
    s >> t;

    return t;
}

template <>
inline double fromString<double>(const std::string &str) {
    if (str == "inf")
        return INFINITY;

    if (str == "nan")
        return NAN;

    std::istringstream s(str);

    double t;
    s >> t;

    return t;
}
