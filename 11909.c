tgoto_internal(const char *string, int x, int y)
{
    static char *result;
    static size_t length;

    int swap_arg;
    int param[3];
    size_t used = 0;
    size_t need = 10;
    int *value = param;
    bool need_BC = FALSE;

    if (BC)
	need += strlen(BC);

    param[0] = y;
    param[1] = x;
    param[2] = 0;

    while (*string != 0) {
	if ((used + need) > length) {
	    length += (used + need);
	    if ((result = typeRealloc(char, length, result)) == 0) {
		length = 0;
		break;
	    }
	}
	if (*string == '%') {
	    const char *fmt = 0;

	    switch (*++string) {
	    case '\0':
		string--;
		break;
	    case 'd':
		fmt = "%d";
		break;
	    case '2':
		fmt = "%02d";
		*value %= 100;
		break;
	    case '3':
		fmt = "%03d";
		*value %= 1000;
		break;
	    case '+':
		*value += UChar(*++string);
		/* FALLTHRU */
	    case '.':
		/*
		 * Guard against tputs() seeing a truncated string.  The
		 * termcap documentation refers to a similar fixup for \n
		 * and \r, but I don't see that it could work -TD
		 */
		if (*value == 0) {
		    if (BC != 0) {
			*value += 1;
			need_BC = TRUE;
		    } else {
			/* tputs will pretend this is \0, which will almost
			 * always work since ANSI-compatible terminals ignore
			 * the character.  ECMA-48 does not document a C1
			 * control for this value.  A few (obsolete) terminals
			 * can use this value in special cases, such as cursor
			 * addressing using single-byte coordinates.
			 */
			*value = 0200;
		    }
		}
		result[used++] = (char) *value++;
		break;
	    case '%':
		result[used++] = *string;
		break;
	    case 'r':
		swap_arg = param[0];
		param[0] = param[1];
		param[1] = swap_arg;
		break;
	    case 'i':
		param[0] += 1;
		param[1] += 1;
		break;
	    case '>':
		if (*value > string[1])
		    *value += string[2];
		string += 2;
		break;
	    case 'n':		/* Datamedia 2500 */
		param[0] ^= 0140;
		param[1] ^= 0140;
		break;
	    case 'B':		/* BCD */
		*value = 16 * (*value / 10) + (*value % 10);
		break;
	    case 'D':		/* Reverse coding (Delta Data) */
		*value -= 2 * (*value % 16);
		break;
	    }
	    if (fmt != 0) {
		_nc_SPRINTF(result + used, _nc_SLIMIT(length - used)
			    fmt, *value++);
		used += strlen(result + used);
		fmt = 0;
	    }
	    if (value - param > 2) {
		value = param + 2;
		*value = 0;
	    }
	} else {
	    result[used++] = *string;
	}
	string++;
    }
    if (result != 0) {
	if (need_BC) {
	    _nc_STRCPY(result + used, BC, length - used);
	    used += strlen(BC);
	}
	result[used] = '\0';
    }
    return result;
}