bytes_subtype_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyObject *tmp, *pnew;
    Py_ssize_t n;

    assert(PyType_IsSubtype(type, &PyBytes_Type));
    tmp = bytes_new(&PyBytes_Type, args, kwds);
    if (tmp == NULL)
        return NULL;
    assert(PyBytes_Check(tmp));
    n = PyBytes_GET_SIZE(tmp);
    pnew = type->tp_alloc(type, n);
    if (pnew != NULL) {
        Py_MEMCPY(PyBytes_AS_STRING(pnew),
                  PyBytes_AS_STRING(tmp), n+1);
        ((PyBytesObject *)pnew)->ob_shash =
            ((PyBytesObject *)tmp)->ob_shash;
    }
    Py_DECREF(tmp);
    return pnew;
}