"""Example python3 application that uses HawkTracer tracing system.

This application demonstrates how to instrument python3 code to measure
execution time of a specific function.

This is the simplest example. In order to enable tracing, it needs to be run
with HT_PYTHON_TRACE_ENABLED=1 environment variable. If the env variable is unset,
tracing is disabled and no overhead is introduced.

"""

from hawktracer.core import trace, register_file_listener

@trace
def foo():
    print("This is foo.")
    print("This function is traced!.")

def bar():
    print("This is bar.")
    print("This function isn't traced, as it doesn't have @trace decorator.")

@trace
def foobar():
    foo()
    print("This is foobar")
    print("This function is also traced!")

register_file_listener("test.htdump", 1024*1024);

for i in range(10):
    foo()
    bar()
    foobar()

