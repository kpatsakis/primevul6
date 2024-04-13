bytes_str(PyObject *op)
{
    if (Py_BytesWarningFlag) {
        if (PyErr_WarnEx(PyExc_BytesWarning,
                         "str() on a bytes instance", 1))
            return NULL;
    }
    return bytes_repr(op);
}