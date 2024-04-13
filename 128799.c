replace_substring_in_place(PyBytesObject *self,
                           const char *from_s, Py_ssize_t from_len,
                           const char *to_s, Py_ssize_t to_len,
                           Py_ssize_t maxcount)
{
    char *result_s, *start, *end;
    char *self_s;
    Py_ssize_t self_len, offset;
    PyBytesObject *result;

    /* The result string will be the same size */

    self_s = PyBytes_AS_STRING(self);
    self_len = PyBytes_GET_SIZE(self);

    offset = stringlib_find(self_s, self_len,
                            from_s, from_len,
                            0);
    if (offset == -1) {
        /* No matches; return the original string */
        return return_self(self);
    }

    /* Need to make a new string */
    result = (PyBytesObject *) PyBytes_FromStringAndSize(NULL, self_len);
    if (result == NULL)
        return NULL;
    result_s = PyBytes_AS_STRING(result);
    Py_MEMCPY(result_s, self_s, self_len);

    /* change everything in-place, starting with this one */
    start =  result_s + offset;
    Py_MEMCPY(start, to_s, from_len);
    start += from_len;
    end = result_s + self_len;

    while ( --maxcount > 0) {
        offset = stringlib_find(start, end-start,
                                from_s, from_len,
                                0);
        if (offset==-1)
            break;
        Py_MEMCPY(start+offset, to_s, from_len);
        start += offset+from_len;
    }

    return result;
}