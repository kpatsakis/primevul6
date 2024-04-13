sameItem(const char *actual, const char *wanted)
{
    Bool result = False;
    size_t have = strlen(actual);
    size_t need = strlen(wanted);

    if (have != 0 && have <= need) {
	if (!strncmp(actual, wanted, have)) {
	    TRACE(("...matched \"%s\"\n", wanted));
	    result = True;
	}
    }

    return result;
}