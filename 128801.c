bytes_richcompare(PyBytesObject *a, PyBytesObject *b, int op)
{
    int c;
    Py_ssize_t len_a, len_b;
    Py_ssize_t min_len;
    PyObject *result;
    int rc;

    /* Make sure both arguments are strings. */
    if (!(PyBytes_Check(a) && PyBytes_Check(b))) {
        if (Py_BytesWarningFlag && (op == Py_EQ || op == Py_NE)) {
            rc = PyObject_IsInstance((PyObject*)a,
                                     (PyObject*)&PyUnicode_Type);
            if (!rc)
                rc = PyObject_IsInstance((PyObject*)b,
                                         (PyObject*)&PyUnicode_Type);
            if (rc < 0)
                return NULL;
            if (rc) {
                if (PyErr_WarnEx(PyExc_BytesWarning,
                                 "Comparison between bytes and string", 1))
                    return NULL;
            }
        }
        result = Py_NotImplemented;
    }
    else if (a == b) {
        switch (op) {
        case Py_EQ:
        case Py_LE:
        case Py_GE:
            /* a string is equal to itself */
            result = Py_True;
            break;
        case Py_NE:
        case Py_LT:
        case Py_GT:
            result = Py_False;
            break;
        default:
            PyErr_BadArgument();
            return NULL;
        }
    }
    else if (op == Py_EQ || op == Py_NE) {
        int eq = bytes_compare_eq(a, b);
        eq ^= (op == Py_NE);
        result = eq ? Py_True : Py_False;
    }
    else {
        len_a = Py_SIZE(a);
        len_b = Py_SIZE(b);
        min_len = Py_MIN(len_a, len_b);
        if (min_len > 0) {
            c = Py_CHARMASK(*a->ob_sval) - Py_CHARMASK(*b->ob_sval);
            if (c == 0)
                c = memcmp(a->ob_sval, b->ob_sval, min_len);
        }
        else
            c = 0;
        if (c == 0)
            c = (len_a < len_b) ? -1 : (len_a > len_b) ? 1 : 0;
        switch (op) {
        case Py_LT: c = c <  0; break;
        case Py_LE: c = c <= 0; break;
        case Py_GT: c = c >  0; break;
        case Py_GE: c = c >= 0; break;
        default:
            PyErr_BadArgument();
            return NULL;
        }
        result = c ? Py_True : Py_False;
    }

    Py_INCREF(result);
    return result;
}