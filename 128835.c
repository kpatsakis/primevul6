bytes_decode(PyObject *self, PyObject *args, PyObject *kwargs)
{
    const char *encoding = NULL;
    const char *errors = NULL;
    static char *kwlist[] = {"encoding", "errors", 0};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "|ss:decode", kwlist, &encoding, &errors))
        return NULL;
    return PyUnicode_FromEncodedObject(self, encoding, errors);
}