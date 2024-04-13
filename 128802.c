bytes_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyObject *x = NULL;
    const char *encoding = NULL;
    const char *errors = NULL;
    PyObject *new = NULL;
    PyObject *func;
    Py_ssize_t size;
    static char *kwlist[] = {"source", "encoding", "errors", 0};
    _Py_IDENTIFIER(__bytes__);

    if (type != &PyBytes_Type)
        return bytes_subtype_new(type, args, kwds);
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|Oss:bytes", kwlist, &x,
                                     &encoding, &errors))
        return NULL;
    if (x == NULL) {
        if (encoding != NULL || errors != NULL) {
            PyErr_SetString(PyExc_TypeError,
                            "encoding or errors without sequence "
                            "argument");
            return NULL;
        }
        return PyBytes_FromString("");
    }

    if (PyUnicode_Check(x)) {
        /* Encode via the codec registry */
        if (encoding == NULL) {
            PyErr_SetString(PyExc_TypeError,
                            "string argument without an encoding");
            return NULL;
        }
        new = PyUnicode_AsEncodedString(x, encoding, errors);
        if (new == NULL)
            return NULL;
        assert(PyBytes_Check(new));
        return new;
    }

    /* If it's not unicode, there can't be encoding or errors */
    if (encoding != NULL || errors != NULL) {
        PyErr_SetString(PyExc_TypeError,
            "encoding or errors without a string argument");
        return NULL;
    }

    /* We'd like to call PyObject_Bytes here, but we need to check for an
       integer argument before deferring to PyBytes_FromObject, something
       PyObject_Bytes doesn't do. */
    func = _PyObject_LookupSpecial(x, &PyId___bytes__);
    if (func != NULL) {
        new = PyObject_CallFunctionObjArgs(func, NULL);
        Py_DECREF(func);
        if (new == NULL)
            return NULL;
        if (!PyBytes_Check(new)) {
            PyErr_Format(PyExc_TypeError,
                         "__bytes__ returned non-bytes (type %.200s)",
                         Py_TYPE(new)->tp_name);
            Py_DECREF(new);
            return NULL;
        }
        return new;
    }
    else if (PyErr_Occurred())
        return NULL;

    /* Is it an integer? */
    size = PyNumber_AsSsize_t(x, PyExc_OverflowError);
    if (size == -1 && PyErr_Occurred()) {
        if (PyErr_ExceptionMatches(PyExc_OverflowError))
            return NULL;
        PyErr_Clear();
    }
    else if (size < 0) {
        PyErr_SetString(PyExc_ValueError, "negative count");
        return NULL;
    }
    else {
        new = PyBytes_FromStringAndSize(NULL, size);
        if (new == NULL)
            return NULL;
        if (size > 0)
            memset(((PyBytesObject*)new)->ob_sval, 0, size);
        return new;
    }

    return PyBytes_FromObject(x);
}