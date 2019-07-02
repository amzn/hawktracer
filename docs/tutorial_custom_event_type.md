# Tutorial: Define custom event type {#tutorial_custom_event_type}
In this tutorial we will learn how to define custom event types.

## Before you start...
We recommend you to go through the @ref tutorial_hello_world tutorial to make sure you have the %HawkTracer library properly installed.

## Problem statement
Let's say we'd like to track all the `fopen()` and `fclose()` calls, and additionally we'd like to know how many files are currently open by the application, whether the operation passed or failed, and what were the arguments.  

## Code
Please copy the text below to a file named `hawktracer-custom-event0tep.c` (you can find this file [in the repository as well](@repocodeurl/examples/tutorials/custom_event_type/hawktracer-custom-event-type.c)).

@include examples/tutorials/custom_event_type/hawktracer-custom-event-type.c

### Building and running the example
You can see @ref integration in order to see how to integrate HawkTracer to your project. For the purpose of the example, here are few shortcuts for most popular platforms:
* **Linux**  
In the terminal, run the following command:
~~~.sh
$ gcc hawktracer-custom-event-type.c $(pkg-config --cflags --libs hawktracer) -o hawktracer-custom-event-type # compiling the code
$ ./hawktracer-custom-event-type # running the example
~~~

[comment]: # (TODO: create section "building examples")

## Walkthrough
We assume you already went through the @ref tutorial_hello_world tutorial, so we only focus here on the new part.

Before we even start writing the logic, in order to define custom event types, we should include an extra header which defines helper macros for defining custom event types:
~~~.c
#include <hawktracer/event_macros_impl.h>
~~~
As discussed earlier, we want to track to operations - open and close file - and for each of the event we want to know how many files we currently have open in the application, whether the operation failed or completed successfully, and what were the parameters of those functions. Since number of opened files and the result are common values for both `open` and `close` operation, we can have a base class with those two fields. However, parameters for `open` and `close` are different, so we create two new classes which inherit from the base class:
~~~.c
HT_DECLARE_EVENT_KLASS(FileOperationEvent, HT_Event,
                       (INTEGER, uint32_t, opened_files_count),
                       (INTEGER, HT_Boolean, success))
HT_DECLARE_EVENT_KLASS(OpenFileEvent, FileOperationEvent,
                       (STRING, const char*, name),
                       (STRING, const char*, mode))
HT_DECLARE_EVENT_KLASS(CloseFileEvent, FileOperationEvent,
                       (STRING, const char*, label))
~~~
HT_DECLARE_EVENT_KLASS() as a first argument takes the type name of the new class, as a second argument - base class, and then list of field definitions. Every field definition consists of data kind (see @ref HT_MKCREFLECT_Types_Ext enum), C type, and the field name. The code generates an actual C struct, e.g. for the `OpenFileEvent`, it is:
~~~.c
typedef struct {
  FileOperationEvent base;
  const char* name;
  const char* mode;
} OpenFileEvent;
~~~
Additionally, the macro defines a few extra helper functions which are used for type registration or data serialization.

Let's move to the main function now. Every new data type must be registered to the %HawkTracer registry, so the library knows how to deal with a particular event. It's done by calling HT_REGISTER_EVENT_KLASS() macro, which takes a class name as a parameter:
~~~.c
HT_REGISTER_EVENT_KLASS(FileOperationEvent);
HT_REGISTER_EVENT_KLASS(OpenFileEvent);
HT_REGISTER_EVENT_KLASS(CloseFileEvent);
~~~
Once we registered our event classes, we can now start using them. %HawkTracer provides a convenient macro HT_TIMELINE_PUSH_EVENT() for creating and pushing events to a timeline:
~~~.c
HT_TIMELINE_PUSH_EVENT(ht_global_timeline_get(), OpenFileEvent, opened_files_count, success, file_name, mode);
HT_TIMELINE_PUSH_EVENT(ht_global_timeline_get(), CloseFileEvent, opened_files_count, success, marker);
~~~
The macro takes 3 arguments:

* a timeline to push the event to
* event type
* a list of values for the particular event

The macro automatically creates an instance of the particular event, and all you need to do is to pass all the values in the order you defined earlier. The macro also sets correct timestamp for you.

## Results
After running the application, it should generate a list following events:
~~~.json
[
  {
    "name": "existing_file",
    "ts": 43138514192,
    "mode": "r",
    "opened_files_count": 1,
    "success": 1
  },
  {
    "name": "create-me.txt",
    "ts": 43138514264,
    "mode": "w",
    "opened_files_count": 2,
    "success": 1
  },
  {
    "label": "fp1",
    "ts": 43138514265,
    "opened_files_count": 2,
    "success": 1
  },
  {
    "name": "non_existing_file",
    "ts": 43138514284,
    "mode": "r",
    "opened_files_count": 1,
    "success": 0
  },
  {
    "label": "fp2",
    "ts": 43138514285,
    "opened_files_count": 1,
    "success": 1
  },
  {
    "label": "fp3",
    "ts": 43138514301,
    "opened_files_count": 0,
    "success": 0
  }
]
~~~
