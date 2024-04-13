do_xstrip(PyBytesObject *self, int striptype, PyObject *sepobj)
{
    Py_buffer vsep;
    char *s = PyBytes_AS_STRING(self);
    Py_ssize_t len = PyBytes_GET_SIZE(self);
    char *sep;
    Py_ssize_t seplen;
    Py_ssize_t i, j;

    if (PyObject_GetBuffer(sepobj, &vsep, PyBUF_SIMPLE) != 0)
        return NULL;
    sep = vsep.buf;
    seplen = vsep.len;

    i = 0;
    if (striptype != RIGHTSTRIP) {
        while (i < len && memchr(sep, Py_CHARMASK(s[i]), seplen)) {
            i++;
        }
    }

    j = len;
    if (striptype != LEFTSTRIP) {
        do {
            j--;
        } while (j >= i && memchr(sep, Py_CHARMASK(s[j]), seplen));
        j++;
    }

    PyBuffer_Release(&vsep);

    if (i == 0 && j == len && PyBytes_CheckExact(self)) {
        Py_INCREF(self);
        return (PyObject*)self;
    }
    else
        return PyBytes_FromStringAndSize(s+i, j-i);
}