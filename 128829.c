bytes_buffer_getbuffer(PyBytesObject *self, Py_buffer *view, int flags)
{
    return PyBuffer_FillInfo(view, (PyObject*)self, (void *)self->ob_sval, Py_SIZE(self),
                             1, flags);
}