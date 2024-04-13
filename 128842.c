bytes_dealloc(PyObject *op)
{
    Py_TYPE(op)->tp_free(op);
}