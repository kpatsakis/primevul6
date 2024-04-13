bytes_startswith(PyBytesObject *self, PyObject *args)
{
    Py_ssize_t start = 0;
    Py_ssize_t end = PY_SSIZE_T_MAX;
    PyObject *subobj;
    int result;

    if (!stringlib_parse_args_finds("startswith", args, &subobj, &start, &end))
        return NULL;
    if (PyTuple_Check(subobj)) {
        Py_ssize_t i;
        for (i = 0; i < PyTuple_GET_SIZE(subobj); i++) {
            result = _bytes_tailmatch(self,
                            PyTuple_GET_ITEM(subobj, i),
                            start, end, -1);
            if (result == -1)
                return NULL;
            else if (result) {
                Py_RETURN_TRUE;
            }
        }
        Py_RETURN_FALSE;
    }
    result = _bytes_tailmatch(self, subobj, start, end, -1);
    if (result == -1) {
        if (PyErr_ExceptionMatches(PyExc_TypeError))
            PyErr_Format(PyExc_TypeError, "startswith first arg must be bytes "
                         "or a tuple of bytes, not %s", Py_TYPE(subobj)->tp_name);
        return NULL;
    }
    else
        return PyBool_FromLong(result);
}