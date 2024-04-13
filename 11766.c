isNumeric(char *source)
{
    char *next = 0;
    long value = strtol(source, &next, 0);
    int result = (next == 0 || next == source || *next != '\0') ? 0 : 1;
    (void) value;
    return result;
}