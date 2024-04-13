bytes_rstrip(PyBytesObject *self, PyObject *args)
{
    if (PyTuple_GET_SIZE(args) == 0)
        return do_strip(self, RIGHTSTRIP); /* Common case */
    else
        return do_argstrip(self, RIGHTSTRIP, args);
}