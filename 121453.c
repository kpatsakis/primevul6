scanTrans(char *source, int *this_is, int *next_is, unsigned *first, unsigned *last)
{
    char ch;
    char *target = source;

    *first = *last = 0;
    if (IsEmpty(target)) {
	target = 0;
    } else {
	do {
	    while (IsSpace(*target))
		target++;
	    *first = (unsigned) (target - source);
	    switch (*this_is = *next_is) {
	    case scanMods:
		while ((ch = *target)) {
		    if (IsSpace(ch)) {
			break;
		    } else if (ch == L_BRACK) {
			*next_is = scanKey;
			break;
		    } else if (ch == ':') {
			*next_is = scanColon;
			break;
		    } else if (ch == '~' && target != source) {
			break;
		    }
		    target++;
		}
		break;
	    case scanKey:
		while ((ch = *target)) {
		    if (IsSpace(ch)) {
			break;
		    } else if (ch == ':') {
			*next_is = scanColon;
			break;
		    }
		    target++;
		    if (ch == R_BRACK)
			break;
		}
		break;
	    case scanColon:
		*next_is = scanFunc;
		target++;
		break;
	    case scanFunc:
		while ((ch = *target)) {
		    if (IsSpace(ch)) {
			break;
		    } else if (ch == L_PAREN) {
			*next_is = scanArgs;
			break;
		    }
		    target++;
		}
		break;
	    case scanArgs:
		while ((ch = *target)) {
		    if (ch == R_PAREN) {
			target++;
			*next_is = scanFunc;
			break;
		    }
		    target++;
		}
		break;
	    }
	    *last = (unsigned) (target - source);
	    if (*target == '\n') {
		*next_is = scanMods;
		target++;
	    }
	} while (*first == *last);
    }
    return target;
}