#include "hawktracer.h"

#include <Python.h>

static PyMethodDef* trace_method = NULL;
static HT_Boolean tracing_enabled = HT_FALSE;
static HT_FileDumpListener* file_listener;
static HT_Boolean is_initialized = HT_FALSE;
static PyObject* tracepoint_map = NULL;

static PyObject*
ht_python_core_trace_function(PyObject* function, PyObject *args);

static PyMethodDef ht_python_core_trace_function_md = {"trace_function", ht_python_core_trace_function, METH_VARARGS, ""};

static uintptr_t
ht_python_core_get_label_address(PyObject* function_name_attr)
{
    if (!PyDict_Contains(tracepoint_map, function_name_attr))
    {
        /* TODO: improve cached string feature so it can handle custom pointers.
        Then we can simply push all the mapping to a listener if it's registered too late. */
        uintptr_t address = (uintptr_t)function_name_attr;
        PyDict_SetItem(tracepoint_map, function_name_attr, PyLong_FromSize_t(address));
        PyObject* function_name = PyUnicode_AsEncodedString(function_name_attr, "UTF-8", "strict");
        HT_TIMELINE_PUSH_EVENT(ht_global_timeline_get(), HT_StringMappingEvent, address, PyBytes_AsString(function_name));
        Py_DECREF(function_name);

        return address;
    }
    else
    {
        return PyLong_AsSize_t(PyDict_GetItem(tracepoint_map, function_name_attr));
    }
}

static PyObject*
ht_python_core_trace_function(PyObject* function, PyObject *args)
{
    uintptr_t address = 0;

    if (!PyCallable_Check(function))
    {
        Py_RETURN_NONE;
    }

    if (tracing_enabled)
    {
        PyObject* function_name_attr = PyObject_GetAttrString(function, "__name__");

        if (function_name_attr)
        {
            address = ht_python_core_get_label_address(function_name_attr);
            ht_feature_callstack_start_int(ht_global_timeline_get(), address);
            Py_DECREF(function_name_attr);
        }
    }

    PyEval_CallObject(function, args);

    if (address)
    {
        ht_feature_callstack_stop(ht_global_timeline_get());
    }

    Py_RETURN_NONE;
}

static PyObject *
ht_python_core_trace(PyObject* Py_UNUSED(self), PyObject* args)
{
    PyObject *traced_function;

    if (!PyArg_ParseTuple(args, "O", &traced_function))
    {
        return NULL;
    }

    if (trace_method)
    {
        return PyCFunction_New(trace_method, traced_function);
    }
    else
    {
        Py_XINCREF(traced_function);
        return traced_function;
    }
}

static PyObject *
ht_python_core_enable_decorator(PyObject* Py_UNUSED(self), PyObject* Py_UNUSED(ignored))
{
    if (!trace_method)
    {
        trace_method = &ht_python_core_trace_function_md;
    }

    Py_RETURN_NONE;
}

static PyObject*
ht_python_core_disable_decorator(PyObject* Py_UNUSED(self), PyObject* Py_UNUSED(ignored))
{
    trace_method = NULL;

    Py_RETURN_NONE;
}

static PyObject*
ht_python_core_enable_tracing(PyObject* Py_UNUSED(self), PyObject* Py_UNUSED(ignored))
{
    tracing_enabled = HT_TRUE;

    Py_RETURN_NONE;
}

static PyObject*
ht_python_core_disable_tracing(PyObject* Py_UNUSED(self), PyObject* Py_UNUSED(ignored))
{
    tracing_enabled = HT_FALSE;

    Py_RETURN_NONE;
}

static int
ht_python_core_clear_module(PyObject* m)
{
    if (is_initialized)
    {
        ht_timeline_flush(ht_global_timeline_get());

        if (file_listener)
        {
            ht_file_dump_listener_stop(file_listener);
        }

        ht_deinit();
    }

    Py_DECREF(tracepoint_map);

    Py_CLEAR(m);
    return 0;
}

static PyObject*
ht_python_core_register_file_listener(PyObject* Py_UNUSED(self), PyObject* args)
{
    const char* file_path;
    unsigned int buffer_size = 4096;
    HT_ErrorCode err;

    if (!is_initialized)
    {
        return PyErr_Format(PyExc_Exception, "HawkTracer library not initialized! init() must be called first.");
    }

    if (!PyArg_ParseTuple(args, "s|I", &file_path, &buffer_size))
    {
        return NULL;
    }

    /* TODO: check if file_listener is already defined, and if so - unregister and destroy it.
       We need ht_timeline_unregister_listener to implement that.
       In the future we might support multiple listeners as well.
       */

    file_listener = ht_file_dump_listener_register(ht_global_timeline_get(), file_path, buffer_size, &err);

    if (!file_listener)
    {
        return PyErr_Format(PyExc_Exception, "Unable to initalize file listener. Error code: %d", err);
    }

    Py_RETURN_NONE;
}

static PyObject*
ht_python_core_init()
{
    if (!is_initialized)
    {
        ht_init(0, NULL);
        is_initialized = HT_TRUE;
    }

    Py_RETURN_NONE;
}

static PyMethodDef HawkTracer_methods[] =
{
    {
        "trace", (PyCFunction)ht_python_core_trace, METH_VARARGS,
        "Attribute that determines whether the time spent in function should be traced."
    },
    {
        "enable_trace_decorator", (PyCFunction)ht_python_core_enable_decorator, METH_NOARGS,
        "Enables @trace decorator. If the decorator is not enabled, adding @trace to the function does not "
        "have any effect, and doesn't introduce any overhead. Code instrumentation can also be enabled "
        "through the HT_PYTHON_TRACE_ENABLED environment variable."
    },
    {
        "disable_trace_decorator", (PyCFunction)ht_python_core_disable_decorator, METH_NOARGS,
        "Disables @trace decorator. See enable_trace_decorator for details."
    },
    {
        "enable_tracing", (PyCFunction)ht_python_core_enable_tracing, METH_NOARGS,
        "Enables tracing. See disable_tracing for details."
    },
    {
        "disable_tracing", (PyCFunction)ht_python_core_disable_tracing, METH_NOARGS,
        "Temporarily disables tracing. The feature is useful if you want to instrument functions in your codebase, "
        "but want to disable the tracing temporarily in a specific place in the code."
    },
    {
        "init", (PyCFunction)ht_python_core_init, METH_NOARGS,
        "Initializes HawkTracer system. This function should not be called if you already initialized "
        "HawkTracer system in your environment, e.g. in your native stack."
    },
    {
        "register_file_listener", (PyCFunction)ht_python_core_register_file_listener, METH_VARARGS,
        "Registers file listener for tracepoints. Calling this function cause all tracepoints to be "
        "saved to a binary file.\n"
        ":param file_path: A path to a file where all the tracepoints will be saved.\n"
        ":param buffer_size: (optional) A size of the internal buffer (default 4096).\n"
    },
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef HawkTracer_Python_Core_module = {
    PyModuleDef_HEAD_INIT,
    "HawkTracer",
    "Utils for writing custom HawkTracer clients.",
    -1,
    HawkTracer_methods,
    NULL,
    NULL,
    ht_python_core_clear_module,
    NULL
};

PyMODINIT_FUNC
PyInit_core(void)
{
    PyObject* module = PyModule_Create(&HawkTracer_Python_Core_module);

    if (module)
    {
        char* tp_state = getenv("HT_PYTHON_TRACE_ENABLED");
        tracing_enabled = tp_state != NULL && strncmp(tp_state, "0", 1) != 0;
        if (tracing_enabled)
        {
            ht_python_core_init();
            trace_method = &ht_python_core_trace_function_md;
        }

        tracepoint_map = PyDict_New();
    }

    return module;
}
