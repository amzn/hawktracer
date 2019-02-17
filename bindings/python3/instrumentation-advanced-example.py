"""Example python3 application that uses HawkTracer tracing system.

This application demonstrates how to instrument python3 code to measure
execution time of a specific function.

This is advanced example that demonstrates all the existing API usage.
See instrumentation-base-example.py for the simplest possible instrumentation
example.
"""

import hawktracer.core

hawktracer.core.init()

# Enable decorators, so @hawktracer.core.trace annotation has an effect
hawktracer.core.enable_trace_decorator()

# Enable tracing. This can also be enabled by setting HT_PYTHON_TRACE_ENABLED environment variable.
hawktracer.core.enable_tracing()

@hawktracer.core.trace
def foo():
    print("This is foo.")
    print("This function is traced, as it has @trace decorator.")

hawktracer.core.disable_trace_decorator()

def bar():
    print("This is bar.")
    print("This function isn't traced, as it doesn't have @trace decorator.")

@hawktracer.core.trace
def foobar():
    foo()
    print("This is foobar")
    print("This function isn't traced even though @trace decorator is added to this function,")
    print("but disable_trace_decorator() was called first.")

hawktracer.core.register_file_listener("test.htdump", 1024*1024);

for i in range(10):
    if i % 2 == 0:
        hawktracer.core.disable_tracing() # temporarily disable tracing

    foo()

    if i % 2 == 0:
        hawktracer.core.enable_tracing()

    bar()
    foobar()

