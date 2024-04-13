PyBytes_FromObject(PyObject *x)
{
    PyObject *new, *it;
    Py_ssize_t i, size;

    if (x == NULL) {
        PyErr_BadInternalCall();
        return NULL;
    }

    if (PyBytes_CheckExact(x)) {
        Py_INCREF(x);
        return x;
    }

    /* Use the modern buffer interface */
    if (PyObject_CheckBuffer(x)) {
        Py_buffer view;
        if (PyObject_GetBuffer(x, &view, PyBUF_FULL_RO) < 0)
            return NULL;
        new = PyBytes_FromStringAndSize(NULL, view.len);
        if (!new)
            goto fail;
        if (PyBuffer_ToContiguous(((PyBytesObject *)new)->ob_sval,
                                  &view, view.len, 'C') < 0)
            goto fail;
        PyBuffer_Release(&view);
        return new;
      fail:
        Py_XDECREF(new);
        PyBuffer_Release(&view);
        return NULL;
    }
    if (PyUnicode_Check(x)) {
        PyErr_SetString(PyExc_TypeError,
                        "cannot convert unicode object to bytes");
        return NULL;
    }

    if (PyList_CheckExact(x)) {
        new = PyBytes_FromStringAndSize(NULL, Py_SIZE(x));
        if (new == NULL)
            return NULL;
        for (i = 0; i < Py_SIZE(x); i++) {
            Py_ssize_t value = PyNumber_AsSsize_t(
                PyList_GET_ITEM(x, i), PyExc_ValueError);
            if (value == -1 && PyErr_Occurred()) {
                Py_DECREF(new);
                return NULL;
            }
            if (value < 0 || value >= 256) {
                PyErr_SetString(PyExc_ValueError,
                                "bytes must be in range(0, 256)");
                Py_DECREF(new);
                return NULL;
            }
            ((PyBytesObject *)new)->ob_sval[i] = (char) value;
        }
        return new;
    }
    if (PyTuple_CheckExact(x)) {
        new = PyBytes_FromStringAndSize(NULL, Py_SIZE(x));
        if (new == NULL)
            return NULL;
        for (i = 0; i < Py_SIZE(x); i++) {
            Py_ssize_t value = PyNumber_AsSsize_t(
                PyTuple_GET_ITEM(x, i), PyExc_ValueError);
            if (value == -1 && PyErr_Occurred()) {
                Py_DECREF(new);
                return NULL;
            }
            if (value < 0 || value >= 256) {
                PyErr_SetString(PyExc_ValueError,
                                "bytes must be in range(0, 256)");
                Py_DECREF(new);
                return NULL;
            }
            ((PyBytesObject *)new)->ob_sval[i] = (char) value;
        }
        return new;
    }

    /* For iterator version, create a string object and resize as needed */
    size = PyObject_LengthHint(x, 64);
    if (size == -1 && PyErr_Occurred())
        return NULL;
    /* Allocate an extra byte to prevent PyBytes_FromStringAndSize() from
       returning a shared empty bytes string. This required because we
       want to call _PyBytes_Resize() the returned object, which we can
       only do on bytes objects with refcount == 1. */
    size += 1;
    new = PyBytes_FromStringAndSize(NULL, size);
    if (new == NULL)
        return NULL;

    /* Get the iterator */
    it = PyObject_GetIter(x);
    if (it == NULL)
        goto error;

    /* Run the iterator to exhaustion */
    for (i = 0; ; i++) {
        PyObject *item;
        Py_ssize_t value;

        /* Get the next item */
        item = PyIter_Next(it);
        if (item == NULL) {
            if (PyErr_Occurred())
                goto error;
            break;
        }

        /* Interpret it as an int (__index__) */
        value = PyNumber_AsSsize_t(item, PyExc_ValueError);
        Py_DECREF(item);
        if (value == -1 && PyErr_Occurred())
            goto error;

        /* Range check */
        if (value < 0 || value >= 256) {
            PyErr_SetString(PyExc_ValueError,
                            "bytes must be in range(0, 256)");
            goto error;
        }

        /* Append the byte */
        if (i >= size) {
            size = 2 * size + 1;
            if (_PyBytes_Resize(&new, size) < 0)
                goto error;
        }
        ((PyBytesObject *)new)->ob_sval[i] = (char) value;
    }
    _PyBytes_Resize(&new, i);

    /* Clean up and return success */
    Py_DECREF(it);
    return new;

  error:
    Py_XDECREF(it);
    Py_XDECREF(new);
    return NULL;
}