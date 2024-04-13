similar_sgr(int num, char *a, char *b)
{
    char *base_a = a;
    char *base_b = b;
    int delaying = 0;

    while (*b != 0) {
	while (*a != *b) {
	    if (*a == 0) {
		if (num < 0) {
		    ;
		} else if (b[0] == '$'
			   && b[1] == '<') {
		    _nc_warning("did not find delay %s", _nc_visbuf(b));
		} else {
		    _nc_warning("checking sgr(%s) %s\n\tcompare to %s\n\tunmatched %s",
				sgr_names[num], _nc_visbuf2(1, base_a),
				_nc_visbuf2(2, base_b),
				_nc_visbuf2(3, b));
		}
		return FALSE;
	    } else if (delaying) {
		a = skip_delay(a);
		b = skip_delay(b);
	    } else if ((*b == '0' || (*b == ';')) && *a == 'm') {
		b++;
	    } else {
		a++;
	    }
	}
	switch (*a) {
	case '$':
	    if (delaying == 0)
		delaying = 1;
	    break;
	case '<':
	    if (delaying == 1)
		delaying = 2;
	    break;
	default:
	    delaying = 0;
	    break;
	}
	a++;
	b++;
    }
    /* ignore delays on the end of the string */
    a = ignore_delays(a);
    return ((num != 0) || (*a == 0));
}