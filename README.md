[![Build Status](https://travis-ci.org/amzn/hawktracer.svg?branch=master)](https://travis-ci.org/amzn/hawktracer)
[![Gitter chat](https://img.shields.io/gitter/room/amzn/hawktracer.svg)](https://gitter.im/amzn/hawktracer)

# HawkTracer

HawkTracer is a highly portable, low-overhead, configurable profiling tool built in Amazon Video for getting performance metrics from low-end devices.

## License Summary

This sample code is made available under the MIT license. 
(See [LICENSE] file)

## Getting Started

### Building
```
$ mkdir build       # It'll be different on Windows
$ cmake ..
$ cmake --build .   # This instead of make, so we don't need extra instructions for Windows
```

## Attaching it to a project to profile

### Adding tracepoints

#### C/C++ code

Use following interface to define scoped tracepoints:

```
HT_TP_GLOBAL_SCOPED_INT(int_label);
HT_TP_GLOBAL_SCOPED_STRING("foo");
```

#### JavaScript

Use JavaScript comments in your code

```
// @TracepointStart Name

// JS code here

// @TracepointStop Name

```

#### Lua

Use Lua pseudo-attributes in your code

```
--@TracepointStart Identifier

-- Lua code here

--@TracepointStop

```

### Add to your project's build system

[TBD]

### Collect the data

[TBD]

### Analyzing the data

* Install google-chrome or chromium browser
* Open the browser, and open chrome://tracing/ webpage
* Click load button and open file generated in the previous section
* You should see a callstack with timing

## Contributing
Please read [CONTRIBUTING.md] for details on our code of conduct, and the process for submitting pull requests to us.

## Acknowledgment
* Hat tip to anyone who's code was used
* Inspiration
* etc
