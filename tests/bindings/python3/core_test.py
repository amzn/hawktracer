import hawktracer.core
import os
import unittest

class TestHawkTracerCore(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super(TestHawkTracerCore, self).__init__(*args, **kwargs)
        self._test_number = 0

    def setUp(self):
        self._test_number = self._test_number +1
        self._file_name = 'hawktracer_python_{}.htdump'.format(self._test_number)
        hawktracer.core.register_file_listener(self._file_name, 32)

    def tearDown(self):
        os.remove(self._file_name)

    def _check_symbol_exists(self, symbol):
        with open(self._file_name, "rb") as f:
            buf = f.read()

        return buf.find(symbol) != -1

    def test_function_should_generate_tracepoints_if_decorator_enabled(self):
        hawktracer.core.enable_tracing()
        hawktracer.core.enable_trace_decorator()

        @hawktracer.core.trace
        def foo():
            pass

        for i in range(100):
            foo()

        self.assertTrue(self._check_symbol_exists(b'foo'))

    def test_return_value_should_be_passed_through_trace_decorator(self):
        hawktracer.core.enable_tracing()
        hawktracer.core.enable_trace_decorator()

        @hawktracer.core.trace
        def foo():
            return 28121991

        self.assertEqual(28121991, foo())

    def test_function_should_not_generate_tracepoints_if_decorator_disabled(self):
        hawktracer.core.enable_tracing()
        hawktracer.core.disable_trace_decorator()

        @hawktracer.core.trace
        def foo():
            pass

        for i in range(100):
            foo()

        self.assertFalse(self._check_symbol_exists(b'foo'))

    def test_function_should_not_generate_tracepoints_if_tracing_disabled(self):
        hawktracer.core.disable_tracing()
        hawktracer.core.enable_trace_decorator()

        @hawktracer.core.trace
        def foo():
            pass

        for i in range(100):
            foo()

        self.assertFalse(self._check_symbol_exists(b'foo'))



if __name__ == '__main__':
    hawktracer.core.init()
    unittest.main()
