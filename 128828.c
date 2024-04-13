bytes_subscript(PyBytesObject* self, PyObject* item)
{
    if (PyIndex_Check(item)) {
        Py_ssize_t i = PyNumber_AsSsize_t(item, PyExc_IndexError);
        if (i == -1 && PyErr_Occurred())
            return NULL;
        if (i < 0)
            i += PyBytes_GET_SIZE(self);
        if (i < 0 || i >= PyBytes_GET_SIZE(self)) {
            PyErr_SetString(PyExc_IndexError,
                            "index out of range");
            return NULL;
        }
        return PyLong_FromLong((unsigned char)self->ob_sval[i]);
    }
    else if (PySlice_Check(item)) {
        Py_ssize_t start, stop, step, slicelength, cur, i;
        char* source_buf;
        char* result_buf;
        PyObject* result;

        if (PySlice_GetIndicesEx(item,
                         PyBytes_GET_SIZE(self),
                         &start, &stop, &step, &slicelength) < 0) {
            return NULL;
        }

        if (slicelength <= 0) {
            return PyBytes_FromStringAndSize("", 0);
        }
        else if (start == 0 && step == 1 &&
                 slicelength == PyBytes_GET_SIZE(self) &&
                 PyBytes_CheckExact(self)) {
            Py_INCREF(self);
            return (PyObject *)self;
        }
        else if (step == 1) {
            return PyBytes_FromStringAndSize(
                PyBytes_AS_STRING(self) + start,
                slicelength);
        }
        else {
            source_buf = PyBytes_AS_STRING(self);
            result = PyBytes_FromStringAndSize(NULL, slicelength);
            if (result == NULL)
                return NULL;

            result_buf = PyBytes_AS_STRING(result);
            for (cur = start, i = 0; i < slicelength;
                 cur += step, i++) {
                result_buf[i] = source_buf[cur];
            }

            return result;
        }
    }
    else {
        PyErr_Format(PyExc_TypeError,
                     "byte indices must be integers, not %.200s",
                     Py_TYPE(item)->tp_name);
        return NULL;
    }
}