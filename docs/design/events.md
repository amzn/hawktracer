# Events {#design_events}

Event is a data structure that carries an information about the system at specific point of time. This could be e.g. resource usge at specific point of time, or duration of operation.

## Base Event properties
User can define event that holds almost any data, however all the event types must inherit from the base event class. That comes with following implications:

* **every event must hold a pointer to a type definition** - event type definition holds information about all the fields and their data types, which is useful for serializing the data into the stream.
* **events have timestamp assigned to them** - we defined an event as a thing that happens at some point of time. We want to keep track on the time when the event happened, therefore all the instances must hold that information.
* **every event is unique** - as the application can generate multiple events, we want to be able to distinguish them. Therefore each event has unique sequence identifier, which allows user to distinguish events that happened at the same time.

To implement all the properties above, we created a base `C` data struct which must be included in all custom events:
~~~.c
struct HT_Event
{
    HT_EventKlass* klass; // pointer to a class definition
    HT_TimestampNs timestamp; // timestamp
    HT_EventId id; // unique identifier
};
~~~

## Defining custom event type
As mentioned above, user can include almost any information in the event. In order to achieve that, we've introduced a concept of *Event inheritance*. It's similar to `C++` inheritance but requires a few extra information that helps us serializing and deserializing events. The library provides macro HT_DECLARE_EVENT_KLASS() to simplify the inheritance process.

The tutorial @ref tutorial_custom_event_type presents how to define custom event types.

### Registering event type
Before using the event type, it must be registered in the system. It's done by calling HT_REGISTER_EVENT_KLASS() at the beginning of your program.

### Field types
When user defines new event type, apart from the C type, she needs to define generic type of the field. The type can be one of:
* `STRUCT` - an event structure (it can't be any data structure, it must be an event type which is registered in the system)
* `STRING` - null-terminated sequence of characters
* `INTEGER` - signed integer number
* `FLOAT` - floating point number
* `DOUBLE` - double-precision floating point number
* `POINTER` - a pointer
* `UNSIGNED_INTEGER` - unsigned integer number

## Scoped tracepoint events
One of the common scenarios for using %HawkTracer is to measure time spent in a specific scope by the application. E.g. user wants to know how long does it take to execute the code inside the `if` block
~~~.c
void foo(int value)
{
    if (value == 0)
    {
        op1();
        op2();
        op3();
        for (int i = 0; i < 10; i++)
        {
            op4();
        }
    }
}
~~~
We could have 2 events, one at the beginning of the `if` statement, and other at the end of the block, but that's very inconvenient. Also, if one of the internal methods throws an exception, we never receive the second event.
To simplify this task, %HawkTracer provides a few macros (see scoped_tracepoint.h and string_scoped_tracepoint.h) and event types (#HT_CallstackStringEvent, #HT_CallstackIntEvent):
~~~.c
void foo(int value)
{
    if (value == 0)
    {
        HT_TP_SCOPED_STRING(timeline, "if (...)");
        op1();
        op2();
        op3();
        for (int i = 0; i < 10; i++)
        {
            op4();
        }
    }
}
~~~
The code above will generate an event of #HT_CallstackStringEvent which holds following information:

* time spent in the scope
* label (`"if (...)"`)
* thread identifier
* timestamp of the first instruction in the block

Additionally, the macro pushes the event to a specified timeline.

[comment]: # (TODO: describe string values in the event object)

