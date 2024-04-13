return_self(PyBytesObject *self)
{
    if (PyBytes_CheckExact(self)) {
        Py_INCREF(self);
        return self;
    }
    return (PyBytesObject *)PyBytes_FromStringAndSize(
        PyBytes_AS_STRING(self),
        PyBytes_GET_SIZE(self));
}