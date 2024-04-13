comma_is_needed(const char *source)
{
    bool result = FALSE;

    if (source != 0) {
	size_t len = strlen(source);
	if (len != 0)
	    result = (source[len - 1] != ',');
    } else {
	result = FALSE;
    }
    return result;
}