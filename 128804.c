PyBytes_Fini(void)
{
    int i;
    for (i = 0; i < UCHAR_MAX + 1; i++)
        Py_CLEAR(characters[i]);
    Py_CLEAR(nullstring);
}