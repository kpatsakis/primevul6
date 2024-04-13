bytes_splitlines(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"keepends", 0};
    int keepends = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|i:splitlines",
                                     kwlist, &keepends))
        return NULL;

    return stringlib_splitlines(
        (PyObject*) self, PyBytes_AS_STRING(self),
        PyBytes_GET_SIZE(self), keepends
        );
}