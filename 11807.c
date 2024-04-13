is_sgr_string(char *value)
{
    bool result = FALSE;

    if (VALID_STRING(value)) {
	int skip = csi_length(value);

	if (skip) {
	    int ch;

	    result = TRUE;
	    value += skip;
	    while ((ch = UChar(*value++)) != '\0') {
		if (isdigit(ch) || ch == ';') {
		    ;
		} else if (ch == 'm' && *value == '\0') {
		    ;
		} else {
		    result = FALSE;
		    break;
		}
	    }
	}
    }
    return result;
}