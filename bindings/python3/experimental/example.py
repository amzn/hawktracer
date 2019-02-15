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

# Test for 2 (decorator that is zero overhead when disabled.)
foo()
bar()

# Test for 3.
hawktracer.enable()
foo()
hawktracer.disable()
foo()

# Footgun! If you import trace, it creates a local binding that won't get changed by enable_decorator() or disable_decorator()
from hawktracer import trace
hawktracer.enable_decorator() # Won't affect the next line.
@trace # Still disabled from above.
def baz():
    pass

