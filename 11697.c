keypad_index(const char *string)
{
    char *test;
    const char *list = "PQRSwxymtuvlqrsPpn";	/* app-keypad except "Enter" */
    int ch;
    long result = -1;

    if ((ch = keypad_final(string)) != '\0') {
	test = (strchr) (list, ch);
	if (test != 0)
	    result = (long) (test - list);
    }
    return result;
}