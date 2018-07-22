#include <Python.h>

#include "client_context.hpp"
#include <hawktracer/client_utils/stream_factory.hpp>

namespace HawkTracer
{
namespace Python
{

using namespace HawkTracer;

struct HT_Python_Client
{
    PyObject_HEAD
    Context* context;
};

static PyObject *
client_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    auto self = reinterpret_cast<HT_Python_Client*>(type->tp_alloc(type, 0));
    if (self)
    {
        const char* source_description;
        PyObject *callback_obj = nullptr;

        if (!PyArg_ParseTuple(args, "s|O", &source_description, &callback_obj))
        {
            Py_DECREF(self);
            return nullptr;
        }

        if (callback_obj && !PyCallable_Check(callback_obj))
        {
            Py_DECREF(self);
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return nullptr;
        }
        auto stream = ClientUtils::make_stream_from_string(source_description);
        if (!stream)
        {
            PyErr_SetString(PyExc_TypeError, "unable to parse source description");
            return nullptr;
        }
        self->context = new Context(std::move(stream), callback_obj);
    }

    return reinterpret_cast<PyObject*>(self);
}

static PyObject *
client_start(HT_Python_Client* self, PyObject *Py_UNUSED(ignored))
{
    self->context->start();
    Py_RETURN_NONE;
}

static PyObject *
client_stop(HT_Python_Client* self, PyObject *Py_UNUSED(ignored))
{
    self->context->stop();
    Py_RETURN_NONE;
}

static PyObject *
client_poll_event(HT_Python_Client* self, PyObject *Py_UNUSED(ignored))
{
    return self->context->poll_event();
}

static void
client_dealloc(PyObject* self)
{
    auto client = reinterpret_cast<HT_Python_Client*>(self);
    delete client->context;
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyMethodDef client_methods[] = {
    {
        "start",  (PyCFunction) client_start, METH_NOARGS,
        "Start listening to new HawkTracer events."
    },
    {
        "stop", (PyCFunction) client_stop, METH_NOARGS,
        "Stop listening to HawkTracer events."
    },
    {
        "poll_event", (PyCFunction) client_poll_event, METH_NOARGS,
        "Check if there's a new event in a queue. If event occured, function "
        "return a tuple where first element is a class name, and second element "
        "is a dictionary: key: event field name; value: field value."
    },
    {nullptr}
};

static PyTypeObject HT_Python_Client_type_object = {
    PyVarObject_HEAD_INIT(NULL, 0)
    const_cast<char*>("HawkTracer.Client"),
    sizeof(HT_Python_Client),
    0,
    client_dealloc,                         /* tp_dealloc */
    0,                                      /* tp_print */
    0,                                      /* tp_getattr */
    0,                                      /* tp_setattr */
    0,                                      /* tp_compare */
    0,                                      /* tp_repr */
    0,                                      /* tp_as_number */
    0,                                      /* tp_as_sequence */
    0,                                      /* tp_as_mapping */
    0,                                      /* tp_hash */
    0,                                      /* tp_call */
    0,                                      /* tp_str */
    0,                                      /* tp_getattro */
    0,                                      /* tp_setattro */
    0,                                      /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags */
    "Class responsible for receiving HawkTracer "
    "events from a specific data source.",  /* tp_doc */
    0,                                      /* tp_traverse */
    0,                                      /* tp_clear */
    0,                                      /* tp_richcompare */
    0,                                      /* tp_weaklistoffset */
    0,                                      /* tp_iter */
    0,                                      /* tp_iternext */
    client_methods,                         /* tp_methods */
    0,                                      /* tp_members */
    0,                                      /* tp_getset */
    0,                                      /* tp_base */
    0,                                      /* tp_dict */
    0,                                      /* tp_descr_get */
    0,                                      /* tp_descr_set */
    0,                                      /* tp_dictoffset */
    0,                                      /* tp_init */
    0,                                      /* tp_alloc */
    client_new,                             /* tp_new */
    0,                                      /* tp_free */
    0,                                      /* tp_is_gc */
    0,                                      /* tp_bases */
    0,                                      /* tp_mro */
    0,                                      /* tp_cache */
    0,                                      /* tp_subclasses */
    0,                                      /* tp_weaklist */
#if PYTHON_API_VERSION >= 1012
    0                                       /* tp_del */
#endif
};

static PyMethodDef HawkTracer_methods[] =
{
    {nullptr, nullptr, 0, nullptr}
};

static struct PyModuleDef HawkTracer_module = {
    PyModuleDef_HEAD_INIT,
    "HawkTracer",
    "Utils for writing custom HawkTracer clients.",
    -1,
    HawkTracer_methods
};

}
}

PyMODINIT_FUNC
PyInit_HawkTracer(void)
{
    if (PyType_Ready(&HawkTracer::Python::HT_Python_Client_type_object) < 0)
    {
        return nullptr;
    }

    PyObject* module = PyModule_Create(&HawkTracer::Python::HawkTracer_module);
    if (module)
    {
        Py_INCREF(&HawkTracer::Python::HT_Python_Client_type_object);
        PyModule_AddObject(module, "Client",
                           (PyObject *) &HawkTracer::Python::HT_Python_Client_type_object);

        if (!PyEval_ThreadsInitialized())
        {
            PyEval_InitThreads();
        }
    }

    return module;
}
