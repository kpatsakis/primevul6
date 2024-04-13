parse_delay_value(const char *src, double *delays, int *always)
{
    int star = 0;

    *delays = 0.0;
    if (always)
	*always = 0;

    while (isdigit(UChar(*src))) {
	(*delays) = (*delays) * 10 + (*src++ - '0');
    }
    if (*src == '.') {
	int gotdot = 1;

	++src;
	while (isdigit(UChar(*src))) {
	    gotdot *= 10;
	    (*delays) += (*src++ - '0') / gotdot;
	}
    }
    while (*src == '*' || *src == '/') {
	if (always == 0 && *src == '/')
	    break;
	if (*src++ == '*') {
	    star = 1;
	} else {
	    *always = 1;
	}
    }
    if (star)
	*delays = -(*delays);
    return src;
}