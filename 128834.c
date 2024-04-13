_bytes_tailmatch(PyBytesObject *self, PyObject *substr, Py_ssize_t start,
                  Py_ssize_t end, int direction)
{
    Py_ssize_t len = PyBytes_GET_SIZE(self);
    Py_ssize_t slen;
    Py_buffer sub_view = {NULL, NULL};
    const char* sub;
    const char* str;

    if (PyBytes_Check(substr)) {
        sub = PyBytes_AS_STRING(substr);
        slen = PyBytes_GET_SIZE(substr);
    }
    else {
        if (PyObject_GetBuffer(substr, &sub_view, PyBUF_SIMPLE) != 0)
            return -1;
        sub = sub_view.buf;
        slen = sub_view.len;
    }
    str = PyBytes_AS_STRING(self);

    ADJUST_INDICES(start, end, len);

    if (direction < 0) {
        /* startswith */
        if (start+slen > len)
            goto notfound;
    } else {
        /* endswith */
        if (end-start < slen || start > len)
            goto notfound;

        if (end-slen > start)
            start = end - slen;
    }
    if (end-start < slen)
        goto notfound;
    if (memcmp(str+start, sub, slen) != 0)
        goto notfound;

    PyBuffer_Release(&sub_view);
    return 1;

notfound:
    PyBuffer_Release(&sub_view);
    return 0;
}