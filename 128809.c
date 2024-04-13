PyBytes_FromString(const char *str)
{
    size_t size;
    PyBytesObject *op;

    assert(str != NULL);
    size = strlen(str);
    if (size > PY_SSIZE_T_MAX - PyBytesObject_SIZE) {
        PyErr_SetString(PyExc_OverflowError,
            "byte string is too long");
        return NULL;
    }
    if (size == 0 && (op = nullstring) != NULL) {
#ifdef COUNT_ALLOCS
        null_strings++;
#endif
        Py_INCREF(op);
        return (PyObject *)op;
    }
    if (size == 1 && (op = characters[*str & UCHAR_MAX]) != NULL) {
#ifdef COUNT_ALLOCS
        one_strings++;
#endif
        Py_INCREF(op);
        return (PyObject *)op;
    }

    /* Inline PyObject_NewVar */
    op = (PyBytesObject *)PyObject_MALLOC(PyBytesObject_SIZE + size);
    if (op == NULL)
        return PyErr_NoMemory();
    (void)PyObject_INIT_VAR(op, &PyBytes_Type, size);
    op->ob_shash = -1;
    Py_MEMCPY(op->ob_sval, str, size+1);
    /* share short strings */
    if (size == 0) {
        nullstring = op;
        Py_INCREF(op);
    } else if (size == 1) {
        characters[*str & UCHAR_MAX] = op;
        Py_INCREF(op);
    }
    return (PyObject *) op;
}