#ifndef HAWKTRACER_VIEWER_LOGGER_HPP
#define HAWKTRACER_VIEWER_LOGGER_HPP

#include <iostream>

class Logger
{
public:
    static void log(const std::string& message)
    {
        std::cerr << message << std::endl;
    }
};

#endif // HAWKTRACER_VIEWER_LOGGER_HPP
