#!usr/bin/env python3

import os
#_tracing_enabled = False
_tracing_enabled = True
if 'HAWKTRACER_TRACE' in os.environ:
    _tracing_enabled = True

def enable():
    global _tracing_enabled
    _tracing_enabled = True

def disable():
    global _tracing_enabled
    _tracing_enabled = False

def _trace(func):
    global _tracing_enabled
    if _tracing_enabled:
        print("Instrumenting function",func.__name__)
        def wrapper(*args, **kwargs):
            _start(func.__name__)
            func(*args, **kwargs)
            _stop()
        return wrapper
    else:
        print("NOT instrumenting function",func.__name__)
        return func

_identity = lambda f:f
trace = _trace

def enable_decorator():
    global trace
    trace = _trace

def disable_decorator():
    global trace
    trace = _identity

def _start(region_name):
    # Actual code that starts a hawktracer region
    pass

def _stop():
    # Actual code that stops a hawktracer region
    pass

def start(region_name):
    if _tracing_enabled:
        print("Started a trace region called",region_name,'...')
        _start(region_name)
    else:
        print("NOT Starting a trace region called",region_name,'...')


def stop():
    if _tracing_enabled:
        print("Stopped a trace region")
        _stop()


