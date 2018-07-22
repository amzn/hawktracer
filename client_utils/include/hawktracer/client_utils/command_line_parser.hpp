#ifndef HAWKTRACER_CLIENT_UTILS_COMMAND_LINE_PARSER_HPP
#define HAWKTRACER_CLIENT_UTILS_COMMAND_LINE_PARSER_HPP

#include <unordered_map>
#include <string>

namespace HawkTracer
{
namespace ClientUtils
{

class CommandLineParser
{
public:
    struct OptionInfo
    {
        OptionInfo(bool is_flag, bool is_mandatory, std::string help_msg) :
            help_msg(std::move(help_msg)), is_flag(is_flag), is_mandatory(is_mandatory) {}

        std::string help_msg;
        bool is_flag;
        bool is_mandatory;
    };

    CommandLineParser(std::string prefix, std::string app_name);

    void register_option(std::string name, OptionInfo option_info);
    bool parse(int argc, char** argv);

    void print_help(std::ostream& stream) const;

    std::string get_value(const std::string& option, std::string default_value) const;
    bool has_value(const std::string& option) const;

private:
    size_t _get_max_option_length(size_t value_help_length) const;
    bool _is_option_valid(const char* option) const;
    bool _validate_user_options() const;

    std::unordered_map<std::string, OptionInfo> _registered_options;
    std::unordered_map<std::string, std::string> _actual_options;
    const std::string _prefix;
    const std::string _app_name;
};

} // namespace ClientUtils
} // namespace HawkTracer

#endif // HAWKTRACER_CLIENT_UTILS_COMMAND_LINE_PARSER_HPP
