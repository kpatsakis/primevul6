bytes_contains(PyObject *self, PyObject *arg)
{
    Py_ssize_t ival = PyNumber_AsSsize_t(arg, PyExc_ValueError);
    if (ival == -1 && PyErr_Occurred()) {
        Py_buffer varg;
        Py_ssize_t pos;
        PyErr_Clear();
        if (PyObject_GetBuffer(arg, &varg, PyBUF_SIMPLE) != 0)
            return -1;
        pos = stringlib_find(PyBytes_AS_STRING(self), Py_SIZE(self),
                             varg.buf, varg.len, 0);
        PyBuffer_Release(&varg);
        return pos >= 0;
    }
    if (ival < 0 || ival >= 256) {
        PyErr_SetString(PyExc_ValueError, "byte must be in range(0, 256)");
        return -1;
    }

    return memchr(PyBytes_AS_STRING(self), (int) ival, Py_SIZE(self)) != NULL;
}