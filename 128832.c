bytes_hash(PyBytesObject *a)
{
    if (a->ob_shash == -1) {
        /* Can't fail */
        a->ob_shash = _Py_HashBytes(a->ob_sval, Py_SIZE(a));
    }
    return a->ob_shash;
}