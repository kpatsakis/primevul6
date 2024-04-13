_PyBytes_Join(PyObject *sep, PyObject *x)
{
    assert(sep != NULL && PyBytes_Check(sep));
    assert(x != NULL);
    return bytes_join(sep, x);
}