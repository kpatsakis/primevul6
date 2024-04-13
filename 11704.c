parse_ti_delay(const char *ti, double *delays)
{
    *delays = 0.0;
    while (*ti != '\0') {
	if (*ti == '\\') {
	    ++ti;
	}
	if (ti[0] == '$'
	    && ti[1] == '<'
	    && IsDelay(UChar(ti[2]))) {
	    int ignored;
	    const char *last = parse_delay_value(ti + 2, delays, &ignored);
	    if (*last == '>') {
		ti = last;
	    }
	} else {
	    ++ti;
	}
    }
    return ti;
}