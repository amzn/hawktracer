#include "ui_controller.hpp"
#include "javascript_ui.hpp"
#include <hawktracer/parser/file_stream.hpp>
#include <hawktracer/parser/protocol_reader.hpp>
#include <hawktracer/parser/make_unique.hpp>

#include "../client/tcp_client_stream.hpp"

#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <source>" << std::endl;
        return 1;
    }

    auto stream =
            HawkTracer::parser::make_unique<HawkTracer::parser::FileStream>(argv[1]);
            //HawkTracer::parser::make_unique<HawkTracer::client::TCPClientStream>("127.0.0.1", 8765);
    HawkTracer::viewer::UIController controller(
                HawkTracer::parser::make_unique<HawkTracer::viewer::JavaScriptUI>(4001), std::move(stream));

    return controller.run();
}
