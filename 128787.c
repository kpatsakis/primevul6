replace_interleave(PyBytesObject *self,
                   const char *to_s, Py_ssize_t to_len,
                   Py_ssize_t maxcount)
{
    char *self_s, *result_s;
    Py_ssize_t self_len, result_len;
    Py_ssize_t count, i;
    PyBytesObject *result;

    self_len = PyBytes_GET_SIZE(self);

    /* 1 at the end plus 1 after every character;
       count = min(maxcount, self_len + 1) */
    if (maxcount <= self_len)
        count = maxcount;
    else
        /* Can't overflow: self_len + 1 <= maxcount <= PY_SSIZE_T_MAX. */
        count = self_len + 1;

    /* Check for overflow */
    /*   result_len = count * to_len + self_len; */
    assert(count > 0);
    if (to_len > (PY_SSIZE_T_MAX - self_len) / count) {
        PyErr_SetString(PyExc_OverflowError,
                        "replacement bytes are too long");
        return NULL;
    }
    result_len = count * to_len + self_len;

    if (! (result = (PyBytesObject *)
                     PyBytes_FromStringAndSize(NULL, result_len)) )
        return NULL;

    self_s = PyBytes_AS_STRING(self);
    result_s = PyBytes_AS_STRING(result);

    /* TODO: special case single character, which doesn't need memcpy */

    /* Lay the first one down (guaranteed this will occur) */
    Py_MEMCPY(result_s, to_s, to_len);
    result_s += to_len;
    count -= 1;

    for (i=0; i<count; i++) {
        *result_s++ = *self_s++;
        Py_MEMCPY(result_s, to_s, to_len);
        result_s += to_len;
    }

    /* Copy the rest of the original string */
    Py_MEMCPY(result_s, self_s, self_len-i);

    return result;
}