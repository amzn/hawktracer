#include <hawktracer/parser/file_stream.hpp>

#include "config.hpp"
#include "pattern_matching.hpp"
#include "patterns.hpp"
#include "source.hpp"

#include <cstring>
#include <iostream>

using namespace HawkTracer;

int main(int argc, char** argv)
{
    std::string config_file;
    std::string patterns_file;
    std::string source_file;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--config") == 0 && i < argc - 1)
        {
            config_file = argv[++i];
        }
        else if (strcmp(argv[i], "--patterns") == 0 && i < argc - 1)
        {
            patterns_file = argv[++i];
        }
        else if (strcmp(argv[i], "--source") == 0 && i < argc - 1)
        {
            source_file = argv[++i];
        }
    }

    auto config = std::make_shared<anomaly::Config>();
    if (!config->init(config_file))
    {
        std::cerr << "Could not open config file" << std::endl;
    }

    auto patterns = std::make_shared<anomaly::Patterns>();
    if (!patterns->init(patterns_file))
    {
        std::cerr << "Could not open patterns file" << std::endl;
    }

    auto source = std::make_shared<anomaly::Source>();
    if (!source->init(source_file))
    {
        std::cerr << "Could not open source file" << std::endl;
    }

    anomaly::PatternMatching pattern_matcher(config, patterns);   
    const auto& sources = source->get_trees();
    for (const auto& source : sources)
    {
        auto ans = pattern_matcher.get_matching_scores(source.first);
        for (auto score : ans)
            std::cout << score << " ";
        std::cout << std::endl;
    }
    return 0;
}
