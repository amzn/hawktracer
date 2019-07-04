%HawkTracer is a highly portable, low-overhead, configurable profiling tool built in Amazon Video for getting performance metrics from low-end devices.

%HawkTracer is available on most popular platforms: Linux, Windows, OS X. The library can be used with C and C++.

The library provides many different types of events (e.g. CPU usage event, duration tracepoint), but the list can easily be extended by the user.

## General
* @subpage build_install
* @subpage tutorials
* @subpage integration
* @subpage design
 * @ref design_library
 * @ref design_client
 * @ref design_htdump_format
* Frequently asked question

## Core features
* Multiplatform support: Linux, macOS, Windows
* Low CPU and memory overhead
* Easy integration with existing projects
* Simple macros for code instrumentation
* Streaming data over TCP/IP protocol or saving them to a file

## Community
Currently the only way to contact the community is to join the Gitter channel [amzn/hawktracer](https://gitter.im/amzn/hawktracer).

For feature requests or bug reports, please use [GitHub form](https://github.com/loganek/hawktracer/issues/new).

## Development
All development on %HawkTracer is done on the [GitHub project](https://github.com/loganek/hawktracer). Instructions for contributing can be found on the @subpage docs/CONTRIBUTING.md page.
