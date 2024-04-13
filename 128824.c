PyBytes_Concat(PyObject **pv, PyObject *w)
{
    PyObject *v;
    assert(pv != NULL);
    if (*pv == NULL)
        return;
    if (w == NULL) {
        Py_CLEAR(*pv);
        return;
    }
    v = bytes_concat(*pv, w);
    Py_DECREF(*pv);
    *pv = v;
}