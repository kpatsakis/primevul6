_PyBytes_Resize(PyObject **pv, Py_ssize_t newsize)
{
    PyObject *v;
    PyBytesObject *sv;
    v = *pv;
    if (!PyBytes_Check(v) || Py_REFCNT(v) != 1 || newsize < 0) {
        *pv = 0;
        Py_DECREF(v);
        PyErr_BadInternalCall();
        return -1;
    }
    /* XXX UNREF/NEWREF interface should be more symmetrical */
    _Py_DEC_REFTOTAL;
    _Py_ForgetReference(v);
    *pv = (PyObject *)
        PyObject_REALLOC((char *)v, PyBytesObject_SIZE + newsize);
    if (*pv == NULL) {
        PyObject_Del(v);
        PyErr_NoMemory();
        return -1;
    }
    _Py_NewReference(*pv);
    sv = (PyBytesObject *) *pv;
    Py_SIZE(sv) = newsize;
    sv->ob_sval[newsize] = '\0';
    sv->ob_shash = -1;          /* invalidate cached hash value */
    return 0;
}