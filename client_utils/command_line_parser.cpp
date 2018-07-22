#include "hawktracer/client_utils/command_line_parser.hpp"

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cassert>

namespace HawkTracer
{
namespace ClientUtils
{

CommandLineParser::CommandLineParser(std::string prefix, std::string app_name) :
    _prefix(std::move(prefix)),
    _app_name(std::move(app_name))
{
    assert(_prefix.length() > 0);
}

void CommandLineParser::register_option(std::string name, OptionInfo option_info)
{
    if (option_info.is_flag && option_info.is_mandatory)
    {
        std::cerr << "Unable to register option " << name
                  << ": option can't be flag and mandatory at the same time" << std::endl;
    }
    else
    {
        _registered_options.insert(std::make_pair(std::move(name), std::move(option_info)));
    }
}

size_t CommandLineParser::_get_max_option_length(size_t value_help_length) const
{
    size_t max_option_length = 0;
    for (const auto& option : _registered_options)
    {
        size_t length = option.first.length();
        if (!option.second.is_flag)
        {
            length += value_help_length;
        }
        if (length > max_option_length)
        {
            max_option_length = length;
        }
    }

    return max_option_length;
}

void CommandLineParser::print_help(std::ostream& stream) const
{
    std::string value_help = " <VALUE>";
    size_t max_option_length = _get_max_option_length(value_help.length());

    stream << "usage: " << _app_name << " [OPTION]..." << std::endl;
    for (const auto& option : _registered_options)
    {
        stream << "  " << _prefix << std::left << std::setw(max_option_length)
               << option.first + (option.second.is_flag ? "" : value_help) << " ";

        if (option.second.is_mandatory)
        {
            stream << "[MANDATORY] ";
        }
        stream << option.second.help_msg << std::endl << std::setw(0);
    }
}

std::string CommandLineParser::get_value(const std::string& option, std::string default_value) const
{
    auto it = _actual_options.find(option);
    return (it != _actual_options.end()) ? it->second : default_value;
}

bool CommandLineParser::has_value(const std::string& option) const
{
    return _actual_options.find(option) != _actual_options.end();
}

bool CommandLineParser::_is_option_valid(const char* option) const
{
    return strlen(option) > _prefix.length() && !strncmp(option, _prefix.c_str(), _prefix.length());
}

bool CommandLineParser::_validate_user_options() const
{
    for (const auto& option : _registered_options)
    {
        if (option.second.is_mandatory && _actual_options.find(option.first) == _actual_options.end())
        {
            std::cerr << "Option " << _prefix << option.first << " must be specified" << std::endl;
            return false;
        }
    }

    return true;
}

bool CommandLineParser::parse(int argc, char** argv)
{
    for (int i = 1; i < argc; i++)
    {
        if (!_is_option_valid(argv[i]))
        {
            std::cerr << "Invalid option: " << argv[i] << std::endl;
            return false;
        }

        std::string option = argv[i] + _prefix.length();

        auto it = _registered_options.find(option);
        if (it == _registered_options.end())
        {
            std::cerr << "Unknown option: " << argv[i] << std::endl;
            return false;
        }

        OptionInfo& info = it->second;
        std::string value;
        if (info.is_flag)
        {
            // leave the value empty
        }
        else if (i == argc - 1)
        {
            std::cerr << argv[i] << " requires a value" << std::endl;
            return false;
        }
        else
        {
            value = argv[++i];
        }

        _actual_options.insert(std::make_pair(std::move(option), std::move(value)));
    }

    return _validate_user_options();
}

} // namespace ClientUtils
} // namespace HawkTracer
