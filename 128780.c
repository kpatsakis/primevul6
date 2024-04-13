bytes_partition(PyBytesObject *self, PyObject *sep_obj)
{
    Py_buffer sep = {NULL, NULL};
    PyObject *res;

    if (PyObject_GetBuffer(sep_obj, &sep, PyBUF_SIMPLE) != 0)
        return NULL;

    res = stringlib_partition(
        (PyObject*) self,
        PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self),
        sep_obj, sep.buf, sep.len
        );
    PyBuffer_Release(&sep);
    return res;
}