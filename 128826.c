striter_reduce(striterobject *it)
{
    if (it->it_seq != NULL) {
        return Py_BuildValue("N(O)n", _PyObject_GetBuiltin("iter"),
                             it->it_seq, it->it_index);
    } else {
        PyObject *u = PyUnicode_FromUnicode(NULL, 0);
        if (u == NULL)
            return NULL;
        return Py_BuildValue("N(N)", _PyObject_GetBuiltin("iter"), u);
    }
}