bytes_find_internal(PyBytesObject *self, PyObject *args, int dir)
{
    PyObject *subobj;
    char byte;
    Py_buffer subbuf;
    const char *sub;
    Py_ssize_t sub_len;
    Py_ssize_t start=0, end=PY_SSIZE_T_MAX;
    Py_ssize_t res;

    if (!stringlib_parse_args_finds_byte("find/rfind/index/rindex",
                                         args, &subobj, &byte, &start, &end))
        return -2;

    if (subobj) {
        if (PyObject_GetBuffer(subobj, &subbuf, PyBUF_SIMPLE) != 0)
            return -2;

        sub = subbuf.buf;
        sub_len = subbuf.len;
    }
    else {
        sub = &byte;
        sub_len = 1;
    }

    if (dir > 0)
        res = stringlib_find_slice(
            PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self),
            sub, sub_len, start, end);
    else
        res = stringlib_rfind_slice(
            PyBytes_AS_STRING(self), PyBytes_GET_SIZE(self),
            sub, sub_len, start, end);

    if (subobj)
        PyBuffer_Release(&subbuf);

    return res;
}