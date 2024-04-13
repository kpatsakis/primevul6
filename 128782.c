do_argstrip(PyBytesObject *self, int striptype, PyObject *args)
{
    PyObject *sep = NULL;

    if (!PyArg_ParseTuple(args, stripformat[striptype], &sep))
        return NULL;

    if (sep != NULL && sep != Py_None) {
        return do_xstrip(self, striptype, sep);
    }
    return do_strip(self, striptype);
}