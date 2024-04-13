bytes_item(PyBytesObject *a, Py_ssize_t i)
{
    if (i < 0 || i >= Py_SIZE(a)) {
        PyErr_SetString(PyExc_IndexError, "index out of range");
        return NULL;
    }
    return PyLong_FromLong((unsigned char)a->ob_sval[i]);
}