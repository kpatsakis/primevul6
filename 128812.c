replace_single_character_in_place(PyBytesObject *self,
                                  char from_c, char to_c,
                                  Py_ssize_t maxcount)
{
    char *self_s, *result_s, *start, *end, *next;
    Py_ssize_t self_len;
    PyBytesObject *result;

    /* The result string will be the same size */
    self_s = PyBytes_AS_STRING(self);
    self_len = PyBytes_GET_SIZE(self);

    next = findchar(self_s, self_len, from_c);

    if (next == NULL) {
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
    start =  result_s + (next-self_s);
    *start = to_c;
    start++;
    end = result_s + self_len;

    while (--maxcount > 0) {
        next = findchar(start, end-start, from_c);
        if (next == NULL)
            break;
        *next = to_c;
        start = next+1;
    }

    return result;
}