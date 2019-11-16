# Tutorial: Hello world! {#tutorial_hello_world}

**Hello world** program is a best way to get a first impression about the software.

%HawkTracer is a profiling tool, so instead of printing it on a terminal, we'll create measurements labeled with *Hello World* string!

After this tutorial you'll know how to get time measurements from your own application.

## Before you start...
Make sure you already have HawkTracer installed in your system. See @ref build_install section to see how to install the project.

## Hello World!
Please copy the text below to a file named `hawktracer-hello-world.c` (you can find this file [in the repository as well](@repocodeurl/examples/tutorials/hello_world/hawktracer-hello-world.c)).

@include examples/tutorials/hello_world/hawktracer-hello-world.c

### Building and running the example
You can see @ref integration in order to see how to integrate HawkTracer to your project. For the purpose of the example, here are few shortcuts for most popular platforms:
* **Linux**
In the terminal, run the following command:
~~~.sh
$ gcc hawktracer-hello-world.c $(pkg-config --cflags --libs hawktracer) -o hawktracer-hello-world # compiling the code
$ ./hawktracer-hello-world # running the example
~~~
Scroll to [Getting the results](#tutorial_hello_world_getting_the_result) section to see how to read and analyze profiling results.


[comment]: # (TODO: create section "building examples")


## Walkthrough
At the very beginning, we have to initialize the library. The function allocates internal buffers, initializes klass registry etc.
~~~.c
/* initialize HawkTracer library */
ht_init(argc, argv);

~~~

%HawkTracer for each measurement generates an %Event. Those events are posted to a timeline. In order to process those events, we have to create timeline listeners, which get events from a timeline. It is possible to write your own listener, but the library already provides few of them. One of existing listeners is HT_FileDumpListener which saves events to a file. As an argument, ht_file_dump_listener_create() takes a file name, and internal buffer size. Parameter `error_code` is optional (i.e. can be `NULL`), and will contain information about failure reason if the listener can't be created.
At the end of the program, the listener should be destroyed.

~~~.c
HT_ErrorCode error_code;
/* Create a listener, it'll handle all the HawkTracer events */
HT_FileDumpListener* listener = ht_file_dump_listener_create("hello-world-out.htdump", 2048, &error_code);
/* Creating listener might fail (e.g. file can't be open),
 * so we have to check the status
 */
if (!listener)
{
    printf("Unable to create listener. Error code: %d\n", error_code);
    ht_deinit();
    return -1;
}
~~~
Once we created a listener, it can finally be registered to a timeline. %HawkTracer allows you to create your own timelines, but provides a global timeline which doesn't require any extra initialization. We register the listener to the global timeline.
~~~.c
ht_timeline_register_listener(ht_global_timeline_get(), ht_file_dump_listener_callback, listener);
~~~

### Code instrumentation
After the initialization, we can instrument the code we want to profile. Pair of functions: ht_feature_callstack_start_string() and ht_feature_callstack_stop() measure the duration of execution of the code between those functions, and label the measurement with the string specified as a second argument.
It's also possible to make nested calls of this functions, as we can see in the example below:
~~~.c
/* We start measuring the code */
ht_feature_callstack_start_string(ht_global_timeline_get(), "hello_world()");
for (int i = 0; i < 100; i++)
{
    /* Another tracepoint, we want to measure printf() function */
    ht_feature_callstack_start_string(ht_global_timeline_get(), "printf()");
    printf("2 * %d = %d\n", i, 2*i);
    /* Stop measurements for printf() */
    ht_feature_callstack_stop(ht_global_timeline_get());
}
/* Stop measurements for the first tracepoint (hello_world()) */
ht_feature_callstack_stop(ht_global_timeline_get());
~~~

#### C++/GNU C instrumentation improvements
For C++ and GNU C compilers %HawkTracer offers macros HT_G_TRACE_FUNCTION() and HT_G_TRACE_OPT_STATIC() which measure the scope of the macro call, e.g. if you put HT_G_TRACE_FUNCTION() at the beginning of your function, it will automatically measure the scope of the entire function. Moreover, macros above generate less data as they use cache feature. (**TODO:** provide a reference for this feature)

The instrumented code above can be replaced with the following:
~~~.cpp
/* A function scoped tracepoint - measures a duration of the scope
 * and automatically labels itself with the function name (i.e. "hello_world")
 */
HT_G_TRACE_FUNCTION()

for (int i = 0; i < 100; i++)
{
    /* Another scoped tracepoint, but we can set the label manually */
    HT_G_TRACE_OPT_STATIC("printf()")
    printf("2 * %d = %d\n", i, 2*i);
}
~~~
A full C++/GNU C example can be found [in the repository](@repocodeurl/examples/tutorials/hello_world/hawktracer-hello-world.cpp).

### Cleanup
All the events pushed to a timeline are buffered, and they're flushed to listeners if the internal buffer of a timeline is full. Since we're about to finish the program, and there still might be some events buffered in a timeline, we have to manually flush the timeline:
~~~.c
/* Flush all the buffered events in a timeline */
ht_timeline_flush(ht_global_timeline_get());
~~~
Before destroying any listener, we need to make sure that it's not used by the library. The safest way to achieve it is to unregister listeners from the timeline:
~~~.c
/* Unregister all the listeners from the global timeline, so we can safely destroy them */
ht_timeline_unregister_all_listeners(ht_global_timeline_get());
~~~
After that we can safely destroy our listener and uninitialize the library.
~~~.c
/* Destroy listeners */
ht_file_dump_listener_destroy(listener);
/* Uninitialize HawkTracer library */
ht_deinit();
~~~

## Getting the results <a name="tutorial_hello_world_getting_the_result"></a>
After compiling and running the code, you should see `hello-world-out.htdump` file. This file contains binary data, and can't be directly used for performance analysis.
%HawkTracer doesn't provide it's own viewer yet, but it provide converters to formats supported existing and well-known viewer tools. For example, in order to convert file to json format (which can be opened by [Catapult Trace Viewer](https://github.com/catapult-project/catapult/tree/master/tracing), run the following command:
~~~.sh
hawktracer-converter --source hello-world-out.htdump --output hello-world-out.json
~~~
Catapult Trace Viewer is provided with Google Chrome and Chromium browsers, so we recommend to install one of them.
Open Google Chrome or Chromium browser, and go to [chrome://tracing](chrome://tracing) URL. In the top left corner you should see *Load* button, which allow you to specify a path to the json file (in our case, it's `hello-world-out.json`). After this, you should see a callstack view of your the program.

![Hello World trace visualization](tutorial_hello_world_trace_out.png)
