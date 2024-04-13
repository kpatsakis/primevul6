bytes_lstrip(PyBytesObject *self, PyObject *args)
{
    if (PyTuple_GET_SIZE(args) == 0)
        return do_strip(self, LEFTSTRIP); /* Common case */
    else
        return do_argstrip(self, LEFTSTRIP, args);
}