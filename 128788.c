hex_digit_to_int(Py_UCS4 c)
{
    if (c >= 128)
        return -1;
    if (Py_ISDIGIT(c))
        return c - '0';
    else {
        if (Py_ISUPPER(c))
            c = Py_TOLOWER(c);
        if (c >= 'a' && c <= 'f')
            return c - 'a' + 10;
    }
    return -1;
}