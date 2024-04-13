bytes_rsplit(PyBytesObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"sep", "maxsplit", 0};
    Py_ssize_t len = PyBytes_GET_SIZE(self), n;
    Py_ssize_t maxsplit = -1;
    const char *s = PyBytes_AS_STRING(self), *sub;
    Py_buffer vsub;
    PyObject *list, *subobj = Py_None;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|On:rsplit",
                                     kwlist, &subobj, &maxsplit))
        return NULL;
    if (maxsplit < 0)
        maxsplit = PY_SSIZE_T_MAX;
    if (subobj == Py_None)
        return stringlib_rsplit_whitespace((PyObject*) self, s, len, maxsplit);
    if (PyObject_GetBuffer(subobj, &vsub, PyBUF_SIMPLE) != 0)
        return NULL;
    sub = vsub.buf;
    n = vsub.len;

    list = stringlib_rsplit((PyObject*) self, s, len, sub, n, maxsplit);
    PyBuffer_Release(&vsub);
    return list;
}