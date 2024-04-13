is_termcap(const char *string)
{
    bool result = TRUE;

    if (string == 0 || *string == '\0') {
	result = FALSE;		/* tparm() handles empty strings */
    } else {
	while ((*string != '\0') && result) {
	    if (*string == '%') {
		switch (*++string) {
		case 'p':
		    result = FALSE;
		    break;
		case '\0':
		    string--;
		    break;
		}
	    } else if (string[0] == '$' && string[1] == '<') {
		result = FALSE;
	    }
	    string++;
	}
    }
    return result;
}