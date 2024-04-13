bytes_fromhex(PyObject *cls, PyObject *args)
{
    PyObject *newstring, *hexobj;
    char *buf;
    Py_ssize_t hexlen, byteslen, i, j;
    int top, bot;
    void *data;
    unsigned int kind;

    if (!PyArg_ParseTuple(args, "U:fromhex", &hexobj))
        return NULL;
    assert(PyUnicode_Check(hexobj));
    if (PyUnicode_READY(hexobj))
        return NULL;
    kind = PyUnicode_KIND(hexobj);
    data = PyUnicode_DATA(hexobj);
    hexlen = PyUnicode_GET_LENGTH(hexobj);

    byteslen = hexlen/2; /* This overestimates if there are spaces */
    newstring = PyBytes_FromStringAndSize(NULL, byteslen);
    if (!newstring)
        return NULL;
    buf = PyBytes_AS_STRING(newstring);
    for (i = j = 0; i < hexlen; i += 2) {
        /* skip over spaces in the input */
        while (PyUnicode_READ(kind, data, i) == ' ')
            i++;
        if (i >= hexlen)
            break;
        top = hex_digit_to_int(PyUnicode_READ(kind, data, i));
        bot = hex_digit_to_int(PyUnicode_READ(kind, data, i+1));
        if (top == -1 || bot == -1) {
            PyErr_Format(PyExc_ValueError,
                         "non-hexadecimal number found in "
                         "fromhex() arg at position %zd", i);
            goto error;
        }
        buf[j++] = (top << 4) + bot;
    }
    if (j != byteslen && _PyBytes_Resize(&newstring, j) < 0)
        goto error;
    return newstring;

  error:
    Py_XDECREF(newstring);
    return NULL;
}