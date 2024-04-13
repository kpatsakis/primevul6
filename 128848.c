PyBytes_AsStringAndSize(PyObject *obj,
                         char **s,
                         Py_ssize_t *len)
{
    if (s == NULL) {
        PyErr_BadInternalCall();
        return -1;
    }

    if (!PyBytes_Check(obj)) {
        PyErr_Format(PyExc_TypeError,
             "expected bytes, %.200s found", Py_TYPE(obj)->tp_name);
        return -1;
    }

    *s = PyBytes_AS_STRING(obj);
    if (len != NULL)
        *len = PyBytes_GET_SIZE(obj);
    else if (strlen(*s) != (size_t)PyBytes_GET_SIZE(obj)) {
        PyErr_SetString(PyExc_TypeError,
                        "expected bytes with no null");
        return -1;
    }
    return 0;
}