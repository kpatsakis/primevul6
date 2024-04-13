ignore_delays(char *s)
{
    int delaying = 0;

    do {
	switch (*s) {
	case '$':
	    if (delaying == 0)
		delaying = 1;
	    break;
	case '<':
	    if (delaying == 1)
		delaying = 2;
	    break;
	case '\0':
	    delaying = 0;
	    break;
	default:
	    if (delaying) {
		s = skip_delay(s);
		if (*s == '>')
		    ++s;
		delaying = 0;
	    }
	    break;
	}
	if (delaying)
	    ++s;
    } while (delaying);
    return s;
}