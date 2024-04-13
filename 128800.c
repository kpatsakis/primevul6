striter_next(striterobject *it)
{
    PyBytesObject *seq;
    PyObject *item;

    assert(it != NULL);
    seq = it->it_seq;
    if (seq == NULL)
        return NULL;
    assert(PyBytes_Check(seq));

    if (it->it_index < PyBytes_GET_SIZE(seq)) {
        item = PyLong_FromLong(
            (unsigned char)seq->ob_sval[it->it_index]);
        if (item != NULL)
            ++it->it_index;
        return item;
    }

    Py_DECREF(seq);
    it->it_seq = NULL;
    return NULL;
}