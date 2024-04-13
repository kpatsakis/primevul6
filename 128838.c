do_strip(PyBytesObject *self, int striptype)
{
    char *s = PyBytes_AS_STRING(self);
    Py_ssize_t len = PyBytes_GET_SIZE(self), i, j;

    i = 0;
    if (striptype != RIGHTSTRIP) {
        while (i < len && Py_ISSPACE(s[i])) {
            i++;
        }
    }

    j = len;
    if (striptype != LEFTSTRIP) {
        do {
            j--;
        } while (j >= i && Py_ISSPACE(s[j]));
        j++;
    }

    if (i == 0 && j == len && PyBytes_CheckExact(self)) {
        Py_INCREF(self);
        return (PyObject*)self;
    }
    else
        return PyBytes_FromStringAndSize(s+i, j-i);
}