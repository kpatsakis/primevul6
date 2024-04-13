bytes_replace(PyBytesObject *self, PyObject *args)
{
    PyObject *res;
    Py_buffer old = {NULL, NULL};
    Py_buffer new = {NULL, NULL};
    Py_ssize_t count = -1;

    if (!PyArg_ParseTuple(args, "y*y*|n:replace", &old, &new, &count))
        return NULL;

    res = (PyObject *)replace((PyBytesObject *) self,
                              (const char *)old.buf, old.len,
                              (const char *)new.buf, new.len, count);

    PyBuffer_Release(&old);
    PyBuffer_Release(&new);
    return res;
}