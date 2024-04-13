bytes_compare_eq(PyBytesObject *a, PyBytesObject *b)
{
    int cmp;
    Py_ssize_t len;

    len = Py_SIZE(a);
    if (Py_SIZE(b) != len)
        return 0;

    if (a->ob_sval[0] != b->ob_sval[0])
        return 0;

    cmp = memcmp(a->ob_sval, b->ob_sval, len);
    return (cmp == 0);
}