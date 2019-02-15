#!/usr/bin/env python3

import hawktracer


hawktracer.enable_decorator()

from time import sleep

@hawktracer.trace
def foo():
    pass

hawktracer.disable_decorator()

@hawktracer.trace
def bar():
    pass

print("Test for 2. (decorator that is zero overhead when disabled. Only foo should be instrumented, not bar.")
foo()
bar()

print("Test for 3. dynamically enabling and disabling individual call sites. There should be one instrumented region here.")
hawktracer.enable()
foo()
hawktracer.disable()
foo()
hawktracer.enable()

# Footgun! If you import trace, it creates a local binding that won't get changed by enable_decorator() or disable_decorator()
from hawktracer import trace
hawktracer.enable_decorator() # Won't affect the next line.
@trace # Still disabled from above.
def baz():
    pass
baz()
