replace_substring(PyBytesObject *self,
                  const char *from_s, Py_ssize_t from_len,
                  const char *to_s, Py_ssize_t to_len,
                  Py_ssize_t maxcount) {
    char *self_s, *result_s;
    char *start, *next, *end;
    Py_ssize_t self_len, result_len;
    Py_ssize_t count, offset;
    PyBytesObject *result;

    self_s = PyBytes_AS_STRING(self);
    self_len = PyBytes_GET_SIZE(self);

    count = stringlib_count(self_s, self_len,
                            from_s, from_len,
                            maxcount);

    if (count == 0) {
        /* no matches, return unchanged */
        return return_self(self);
    }

    /* Check for overflow */
    /*    result_len = self_len + count * (to_len-from_len) */
    assert(count > 0);
    if (to_len - from_len > (PY_SSIZE_T_MAX - self_len) / count) {
        PyErr_SetString(PyExc_OverflowError,
                        "replacement bytes are too long");
        return NULL;
    }
    result_len = self_len + count * (to_len-from_len);

    if ( (result = (PyBytesObject *)
          PyBytes_FromStringAndSize(NULL, result_len)) == NULL)
        return NULL;
    result_s = PyBytes_AS_STRING(result);

    start = self_s;
    end = self_s + self_len;
    while (count-- > 0) {
        offset = stringlib_find(start, end-start,
                                from_s, from_len,
                                0);
        if (offset == -1)
            break;
        next = start+offset;
        if (next == start) {
            /* replace with the 'to' */
            Py_MEMCPY(result_s, to_s, to_len);
            result_s += to_len;
            start += from_len;
        } else {
            /* copy the unchanged old then the 'to' */
            Py_MEMCPY(result_s, start, next-start);
            result_s += (next-start);
            Py_MEMCPY(result_s, to_s, to_len);
            result_s += to_len;
            start = next+from_len;
        }
    }
    /* Copy the remainder of the remaining string */
    Py_MEMCPY(result_s, start, end-start);

    return result;
}