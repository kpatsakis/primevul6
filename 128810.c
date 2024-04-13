bytes_repeat(PyBytesObject *a, Py_ssize_t n)
{
    Py_ssize_t i;
    Py_ssize_t j;
    Py_ssize_t size;
    PyBytesObject *op;
    size_t nbytes;
    if (n < 0)
        n = 0;
    /* watch out for overflows:  the size can overflow int,
     * and the # of bytes needed can overflow size_t
     */
    if (n > 0 && Py_SIZE(a) > PY_SSIZE_T_MAX / n) {
        PyErr_SetString(PyExc_OverflowError,
            "repeated bytes are too long");
        return NULL;
    }
    size = Py_SIZE(a) * n;
    if (size == Py_SIZE(a) && PyBytes_CheckExact(a)) {
        Py_INCREF(a);
        return (PyObject *)a;
    }
    nbytes = (size_t)size;
    if (nbytes + PyBytesObject_SIZE <= nbytes) {
        PyErr_SetString(PyExc_OverflowError,
            "repeated bytes are too long");
        return NULL;
    }
    op = (PyBytesObject *)PyObject_MALLOC(PyBytesObject_SIZE + nbytes);
    if (op == NULL)
        return PyErr_NoMemory();
    (void)PyObject_INIT_VAR(op, &PyBytes_Type, size);
    op->ob_shash = -1;
    op->ob_sval[size] = '\0';
    if (Py_SIZE(a) == 1 && n > 0) {
        memset(op->ob_sval, a->ob_sval[0] , n);
        return (PyObject *) op;
    }
    i = 0;
    if (i < size) {
        Py_MEMCPY(op->ob_sval, a->ob_sval, Py_SIZE(a));
        i = Py_SIZE(a);
    }
    while (i < size) {
        j = (i <= size-i)  ?  i  :  size-i;
        Py_MEMCPY(op->ob_sval+i, op->ob_sval, j);
        i += j;
    }
    return (PyObject *) op;
}