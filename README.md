[![Build Status](https://travis-ci.org/amzn/hawktracer.svg?branch=master)](https://travis-ci.org/amzn/hawktracer)
[![Gitter chat](https://img.shields.io/gitter/room/amzn/hawktracer.svg)](https://gitter.im/amzn/hawktracer)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/8d0f18c7a6b14f15bb747b0a4cd336bb)](https://www.codacy.com/app/loganek/hawktracer?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=loganek/hawktracer&amp;utm_campaign=Badge_Grade)
[![codecov](https://codecov.io/gh/loganek/hawktracer/branch/master/graph/badge.svg)](https://codecov.io/gh/loganek/hawktracer)
[![Build status](https://ci.appveyor.com/api/projects/status/3yh0rvyxs0x38802?svg=true)](https://ci.appveyor.com/project/loganek/hawktracer)

# HawkTracer

HawkTracer is a highly portable, low-overhead, configurable profiling tool built in Amazon Video for getting performance metrics from low-end devices.

HawkTracer is available on most popular platforms: **Linux**, **Windows**, **OS X**. By default, the library can be used with **C** and **C++**, but there are also wrappers for [**Python**](bindings/python3) and [**Rust**](https://github.com/AlexEne/rust_hawktracer).

The library provides many different types of events (e.g. CPU usage event, duration tracepoint), but the list can easily be extended by the user.

## Features
* Low CPU/memory overhead
* Multi-platform
* Highly configurable on runtime
* Easy build integration (CMake & pkg-config files)
* Pre-defined and user-defined events
* Simple macros for code instrumentation
* Streaming events to file, through TCP/IP protocol, or handling them in custom function
* Client for receiving event stream
* Library for parsing event stream (so you can write your own client)

## License Summary

This sample code is made available under the MIT license.
(See [LICENSE](LICENSE) file)

## Getting Started

### Hello World! tutorials
See [the tutorial](https://www.hawktracer.org/doc/stable/tutorial_hello_world.html) to quickly onboard to HawkTracer or check the [examples](examples/) directory.
The tutorial step by step explains how to build and instrument the code like that:
```cpp
static void hello_world()
{
    HT_G_TRACE_FUNCTION()

    for (int i = 0; i < 100; i++)
    {
        HT_TP_G_STRACEPOINT("iteration")
        printf("2 * %d = %d\n", i, 2*i);

        std::string txt = "Iteration (mod 10): " + std::to_string(i % 10);
        HT_TP_G_DYN_STRACEPOINT(txt.c_str());
        printf("text: %s\n", txt.c_str());
    }
}
```
You can also follow instructions below to get HawkTracer up and running.

### Building library
```bash
$ mkdir build       # It'll be different on Windows
$ cd build
$ cmake ..
$ cmake --build .   # This instead of make, so we don't need extra instructions for Windows
```

## Attaching HawkTracer to a project to profile

### Build system integration

#### CMake-based projects
If you use CMake build system, you can use following code to attach HawkTracer library to your project:
```cmake
project(your_project)

# optionally, you might define a path to HawkTracer's CMake module
# CMake the path below should be a path to a directory where HawkTracerConfig.cmake is located, e.g.:
# list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake/HawkTracer")

find_package(HawkTracer REQUIRED)

add_executable(your_project main.cc)

target_link_libraries(your_project HawkTracer::hawktracer)

```

#### pkg-config
HawkTracer library provides pkg-config file which can be used to find required libraries and include paths. You can simply integrate it e.g. with your compilation command:
```bash
$ g++ my_project.cpp $(pkg-config --cflags --libs hawktracer)
```

### Instrumenting code

#### Initialize library

Before you start profiling your code, you need to initialize HawkTracer library. There are 2 functions which always have to be called in projects profiled by HawkTracer: `ht_init` and `ht_deinit`.
Additionally, you need to specify an event listener. HawkTracer currently provides 2 listeners:
* TCP listener, which streams events over the network
* File dump listener, which saves events to a file.

Moreover, HawkTracer allows to provide user-defined listeners.
Code below presents example HawkTracer (un)initialization:
```cpp
int main(int argc, char** argv)
{
  ht_init(argc, argv); // initialize library
  HT_Timeline* timeline = ht_global_timeline_get(); // timeline, where all events are posted. You can define your own timeline, or use global timeline
  const size_t buffer_size = 4096; // size of internal listener's buffer
  ht_file_dump_listener_register(ht_global_timeline_get(), "hello-world-out.htdump", 2048, NULL); // create listener and attach it to timeline

  // your code goes here...

  ht_deinit(); // uninitialize library

  return 0;
}

```
The code registers file dump listener, which saves all the events to a file `file_name.htdump`. The file should be then converted to a viewer's format ([see here for details](#collect-the-data)).

#### Instrumenting the code
HawkTracer requires explicit code instrumentation. The library provides a few helper macros for reporting data to a timeline:
```cpp
// Pushes any type of event to a timeline
HT_TIMELINE_PUSH_EVENT(TIMELINE, EVENT_TYPE, EVENT_PARAMETERS,...)

// Reports a duration of specific block of code (available only for C++ or C GNU compiler)
HT_TRACE(TIMELINE, LABEL)

// The same as above, but automatically sets label to current function name
HT_TRACE_FUNCTION(TIMELINE)
```
There are few macros which report events to a global timeline, they're prefixed with `G_`:
```cpp
HT_G_TRACE(LABEL)
HT_G_TRACE_OPT_STATIC(LABEL)
HT_G_TRACE_OPT_DYNAMIC(LABEL)
HT_G_TRACE_FUNCTION()
```
For example, you can instrument following code:
```cpp
void foo()
{
  HT_G_TRACE_FUNCTION() // measure duration of foo function execution

  for (int i = 0; i < 100; i++)
  {
    HT_G_TRACE_OPT_STATIC("in-loop") // measure duration of single loop iteration
    bar();
  }
}
```

### Collect the data

For now HawkTracer provides a simple application for converting event stream to a JSON format which can be consumed by the viewing application: `hawktracer-to-json`. You can use it with a file or with a network stream. Assuming your events have been saved to `file_name.htdump` file, you can generate the JSON file running following command:
```bash
hawktracer-to-json --source file_name.htdump --output output_file.json
```

### Analyzing the data

* Install google-chrome or chromium browser
* Open the browser, and open [chrome://tracing/](chrome://tracing) webpage
* Click load button and open file generated in the previous section
* You should see a callstack with timing

## Contributing
Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests to us.

## Acknowledgment
* Hat tip to anyone who's code was used
* Inspiration
* etc
