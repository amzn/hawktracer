#include "ui_controller.hpp"
#include "javascript_ui.hpp"
#include <parser/file_stream.hpp>
#include <parser/protocol_reader.hpp>
#include <parser/make_unique.hpp>

#include "../client/tcp_client_stream.hpp"

#include <unistd.h>
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
    HawkTracer::parser::ProtocolReader reader(std::move(stream), true);
    HawkTracer::viewer::UIController controller(HawkTracer::parser::make_unique<HawkTracer::viewer::JavaScriptUI>(4001));

    reader.register_events_listener([&controller] (const HawkTracer::parser::Event& event) {
        controller.handle_event(event);
    });

    reader.start();

    sleep(1000);

    reader.wait_for_complete();

    return 0;
}
