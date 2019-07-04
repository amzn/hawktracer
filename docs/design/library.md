# Core library design {#design_library}
The %HawkTracer core library provides an API for reporting events and forwarding them to listeners. The diagram below presents base components of the diagram and their relations.

![HawkTracer core library high level design diagram](library_high_level_design.png)

As shown on the diagram, there's a few core components in the library, which are described below in details:

* `HT_Timeline` - a main class of the library - it's used as an event buffer. User needs to register listeners which are notified when the timeline's buffer is full.
In theory, the HT_Timeline class can be extended by inheritance, however, the recommended way of adding extra features to it is by creating and registering features - that allows user to easily add selected features to his timeline (inheritance allows that too, but that creates complex inheritance dependencies).
Depending on the use case, `HT_Timeline` objects can be thread safe, but the recommended way is to have per-thread timeline instance with shared group of listeners - this pattern is implemented by a global timeline (ht_global_timeline_get()), which is a special instance of timeline provided by the library.
See the timeline.h for the full API.
* `Timeline Feature` - defines an extension for the timeline. The feature should be used whenever there's a need for adding an extran information and operations to the timeline.
For example, callstack feature allows to measure an execution time of scoped operations. To implement that feature, we needed to have an instance of the stack attached to a timeline. Since the `HT_Timeline` structure can't be modified, we implemented a feature which holds a stack internally.
* `Listener` - it's a callback and user data attached to it, which gets called when the timeline's buffer is full. Listener's responsibility is to process events - in most of the scenarios the listener just stores the events so they can be analysed later.
* `HT_Event` - base data type in the library. The base type holds information about the klass of the instance, timestamp, and sequence number. This type can (and should) be extended as most of the time user needs to attach more data to an event (e.g. duration, cpu usage etc.).
* `HT_EventKlass` - contains information about the event. Library creates one instance of this class per type, and all the `HT_Event` instances holds the pointer to it. The class is used for serializing/deserializing events as it holds information about all the fields and their types (it's achieved by using [MKCREFLECT](https://github.com/loganek/mkcreflect) library, which is also maintained by %HawkTracer's authors).
* `Registry` - holds information about all the timeline features and event classes. It provides an interface for registering new data types, but also for quering existing defined types. See registry.h for details and the API.

[comment]: # (TODO: add sequence diagram)")

## Library interface language
As a language for the interface of the library we've choosen C language, and the interface is compatible with the standard C99. The reason we've chosen the language is because of compatibility with other programming languages - most of the popular high-level programming languages allow to implement bindings for C library. We wanted %HawkTracer to be usable in cross-language projects, therefore ability for writing bindings was one of the base design requirements.
It's already been proven that %HawkTracer API is bindable and usable in other languages, as bindings for 2 languages already exist:

 * Python bindings - [the bindings implementation](https://github.com/amzn/hawktracer/blob/master/bindings/python3/hawktracer_core_python.cpp) and [example usage](https://github.com/amzn/hawktracer/blob/master/bindings/python3/instrumentation-base-example.py) is included in official %HawkTracer repository
 * Rust bindings - [rust_hawktracer](https://github.com/AlexEne/rust_hawktracer) is an OpenSource implementation of %HawkTracer bindings for [Rust programming language](https://www.rust-lang.org). The project was also been presented on [FOSDEM conference](https://fosdem.org/2019/schedule/event/rust_hawktracer/).

The library also exposes some C++ and non-C99 (e.g. GNU C) interfaces (e.g. for scoped tracepoints we use C++ destructors and GNU C `__cleanup__` attributes), but those features are hidden if the compiler doesn't support it so the compilation doesn't fail.

## Event inheritance
Event is a structure for all the data that user wants to log. The base data structure, `HT_Event`, only provides information about the timestamp and event data type, but it usually is not enough for the user.
`HT_Event` type is used as a base class for the event. User can create any hierarchy of events, and the library itself uses that already for callstack tracepoints. There's a base callstack tracepoint class for holding the duration and thread identifier, but depending on the use case, the label should be either a number, or a string, so we define two different events for that (see core_events.h for an actual implementation).

![HawkTracer event inheritance diagram](event_inheritance.png)

The tutorial @ref tutorial_custom_event_type presents how to define custom event types.

## Platform-specific features
Some of the features, like memory usage, are platform specific, and it's difficult to provide all possible implementations. Therefore %HawkTracer only implements those features for a few common platforms (usually Linux and Win32), and leaves the interface not implemented for other platforms. If the implementation is not available for user platform, it's possible to provide the implementation for %HawkTracer, or leave it not implemented - in that case, the interface can't be used. An attempt of using not implemented interface will result with the linker error.
For now, %HawkTracer defines following platform-specific features:

* CPU usage - cpu_usage.h file
* Memory usage - memory_usage.h file

## Task scheduler
Some of the operations require periodical execution. %HawkTracer provides a class @ref HT_TaskScheduler which can be used for scheduling periodic tasks.
Please note %HawkTracer does not provide any runtime, therefore it's user responsibility to poll task scheduler (i.e. execute ht_task_scheduler_tick() method).

## Scoped tracepoints
One of the most common usage of the %HawkTracer library is to measure execution time of the scope. This normally is achieved by a pair of `start` and `stop` functions. %HawkTracer provides a few helper macros that automatically figure out the scope based on the usage and generate an approperiate event.
See the tutorial @ref tutorial_hello_world for examples of using scoped tracepoints.

## Custom memory allocator
%HawkTracer tries to avoid allocation on the heap if possible, however, there are places where the library must allocate dynamic memory. By default, the library uses standard C methods for allocation (`malloc()`, `realloc()`, `free()`), but it provides a mechanism for overriding those methods and provide custom allocator.
See alloc.h file for details.

## Thread safety
Most of the functions in the library are thread safe, i.e. they can be executed from any thread, at any point of time. However, that's not the case for Timeline. Pushing events to a timeline is very frequent operation and enabling thread safety might have significant performance impact, therefore we let user decide, whether thread safety for the timeline should be enabled or not. Moreover, some of the timeline features might not be thread-safe at all (e.g. `callstack feature`) as they don't make sense to be used in multiple threads, and some of them have configuration parameter (e.g. `cached string feature`).
