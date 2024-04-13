bytes_translate(PyBytesObject *self, PyObject *args)
{
    char *input, *output;
    Py_buffer table_view = {NULL, NULL};
    Py_buffer del_table_view = {NULL, NULL};
    const char *table;
    Py_ssize_t i, c, changed = 0;
    PyObject *input_obj = (PyObject*)self;
    const char *output_start, *del_table=NULL;
    Py_ssize_t inlen, tablen, dellen = 0;
    PyObject *result;
    int trans_table[256];
    PyObject *tableobj, *delobj = NULL;

    if (!PyArg_UnpackTuple(args, "translate", 1, 2,
                          &tableobj, &delobj))
        return NULL;

    if (PyBytes_Check(tableobj)) {
        table = PyBytes_AS_STRING(tableobj);
        tablen = PyBytes_GET_SIZE(tableobj);
    }
    else if (tableobj == Py_None) {
        table = NULL;
        tablen = 256;
    }
    else {
        if (PyObject_GetBuffer(tableobj, &table_view, PyBUF_SIMPLE) != 0)
            return NULL;
        table = table_view.buf;
        tablen = table_view.len;
    }

    if (tablen != 256) {
        PyErr_SetString(PyExc_ValueError,
          "translation table must be 256 characters long");
        PyBuffer_Release(&table_view);
        return NULL;
    }

    if (delobj != NULL) {
        if (PyBytes_Check(delobj)) {
            del_table = PyBytes_AS_STRING(delobj);
            dellen = PyBytes_GET_SIZE(delobj);
        }
        else {
            if (PyObject_GetBuffer(delobj, &del_table_view, PyBUF_SIMPLE) != 0) {
                PyBuffer_Release(&table_view);
                return NULL;
            }
            del_table = del_table_view.buf;
            dellen = del_table_view.len;
        }
    }
    else {
        del_table = NULL;
        dellen = 0;
    }

    inlen = PyBytes_GET_SIZE(input_obj);
    result = PyBytes_FromStringAndSize((char *)NULL, inlen);
    if (result == NULL) {
        PyBuffer_Release(&del_table_view);
        PyBuffer_Release(&table_view);
        return NULL;
    }
    output_start = output = PyBytes_AsString(result);
    input = PyBytes_AS_STRING(input_obj);

    if (dellen == 0 && table != NULL) {
        /* If no deletions are required, use faster code */
        for (i = inlen; --i >= 0; ) {
            c = Py_CHARMASK(*input++);
            if (Py_CHARMASK((*output++ = table[c])) != c)
                changed = 1;
        }
        if (!changed && PyBytes_CheckExact(input_obj)) {
            Py_INCREF(input_obj);
            Py_DECREF(result);
            result = input_obj;
        }
        PyBuffer_Release(&del_table_view);
        PyBuffer_Release(&table_view);
        return result;
    }

    if (table == NULL) {
        for (i = 0; i < 256; i++)
            trans_table[i] = Py_CHARMASK(i);
    } else {
        for (i = 0; i < 256; i++)
            trans_table[i] = Py_CHARMASK(table[i]);
    }
    PyBuffer_Release(&table_view);

    for (i = 0; i < dellen; i++)
        trans_table[(int) Py_CHARMASK(del_table[i])] = -1;
    PyBuffer_Release(&del_table_view);

    for (i = inlen; --i >= 0; ) {
        c = Py_CHARMASK(*input++);
        if (trans_table[c] != -1)
            if (Py_CHARMASK(*output++ = (char)trans_table[c]) == c)
                continue;
        changed = 1;
    }
    if (!changed && PyBytes_CheckExact(input_obj)) {
        Py_DECREF(result);
        Py_INCREF(input_obj);
        return input_obj;
    }
    /* Fix the size of the resulting string */
    if (inlen > 0)
        _PyBytes_Resize(&result, output - output_start);
    return result;
}