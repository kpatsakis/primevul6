bytes_rfind(PyBytesObject *self, PyObject *args)
{
    Py_ssize_t result = bytes_find_internal(self, args, -1);
    if (result == -2)
        return NULL;
    return PyLong_FromSsize_t(result);
}