_nc_captoinfo(const char *cap, const char *s, int const parameterized)
{
    const char *capstart;

    stackptr = 0;
    onstack = 0;
    seenm = 0;
    seenn = 0;
    seenr = 0;
    param = 1;

    DEBUG_THIS(("_nc_captoinfo params %d, %s", parameterized, s));

    dp = init_string();

    /* skip the initial padding (if we haven't been told not to) */
    capstart = 0;
    if (s == 0)
	s = "";
    if (parameterized >= 0 && isdigit(UChar(*s)))
	for (capstart = s; *s != '\0'; s++)
	    if (!(isdigit(UChar(*s)) || *s == '*' || *s == '.'))
		break;

    while (*s != '\0') {
	switch (*s) {
	case '%':
	    s++;
	    if (parameterized < 1) {
		dp = save_char(dp, '%');
		break;
	    }
	    switch (*s++) {
	    case '%':
		dp = save_string(dp, "%%");
		break;
	    case 'r':
		if (seenr++ == 1) {
		    _nc_warning("saw %%r twice in %s", cap);
		}
		break;
	    case 'm':
		if (seenm++ == 1) {
		    _nc_warning("saw %%m twice in %s", cap);
		}
		break;
	    case 'n':
		if (seenn++ == 1) {
		    _nc_warning("saw %%n twice in %s", cap);
		}
		break;
	    case 'i':
		dp = save_string(dp, "%i");
		break;
	    case '6':
	    case 'B':
		getparm(param, 1);
		dp = save_string(dp, "%{10}%/%{16}%*");
		getparm(param, 1);
		dp = save_string(dp, "%{10}%m%+");
		break;
	    case '8':
	    case 'D':
		getparm(param, 2);
		dp = save_string(dp, "%{2}%*%-");
		break;
	    case '>':
		/* %?%{x}%>%t%{y}%+%; */
		if (s[0] && s[1]) {
		    getparm(param, 2);
		    dp = save_string(dp, "%?");
		    s += cvtchar(s);
		    dp = save_string(dp, "%>%t");
		    s += cvtchar(s);
		    dp = save_string(dp, "%+%;");
		} else {
		    _nc_warning("expected two characters after %%>");
		    dp = save_string(dp, "%>");
		}
		break;
	    case 'a':
		if ((*s == '=' || *s == '+' || *s == '-'
		     || *s == '*' || *s == '/')
		    && (s[1] == 'p' || s[1] == 'c')
		    && s[2] != '\0') {
		    int l;
		    l = 2;
		    if (*s != '=')
			getparm(param, 1);
		    if (s[1] == 'p') {
			getparm(param + s[2] - '@', 1);
			if (param != onstack) {
			    pop();
			    param--;
			}
			l++;
		    } else
			l += cvtchar(s + 2);
		    switch (*s) {
		    case '+':
			dp = save_string(dp, "%+");
			break;
		    case '-':
			dp = save_string(dp, "%-");
			break;
		    case '*':
			dp = save_string(dp, "%*");
			break;
		    case '/':
			dp = save_string(dp, "%/");
			break;
		    case '=':
			if (seenr) {
			    if (param == 1)
				onstack = 2;
			    else if (param == 2)
				onstack = 1;
			    else
				onstack = param;
			} else
			    onstack = param;
			break;
		    }
		    s += l;
		    break;
		}
		getparm(param, 1);
		s += cvtchar(s);
		dp = save_string(dp, "%+");
		break;
	    case '+':
		getparm(param, 1);
		s += cvtchar(s);
		dp = save_string(dp, "%+%c");
		pop();
		break;
	    case 's':
#ifdef WATERLOO
		s += cvtchar(s);
		getparm(param, 1);
		dp = save_string(dp, "%-");
#else
		getparm(param, 1);
		dp = save_string(dp, "%s");
		pop();
#endif /* WATERLOO */
		break;
	    case '-':
		s += cvtchar(s);
		getparm(param, 1);
		dp = save_string(dp, "%-%c");
		pop();
		break;
	    case '.':
		getparm(param, 1);
		dp = save_string(dp, "%c");
		pop();
		break;
	    case '0':		/* not clear any of the historical termcaps did this */
		if (*s == '3') {
		    ++s;
		    goto see03;
		}
		if (*s == '2') {
		    ++s;
		    goto see02;
		}
		goto invalid;
	    case '2':
	      see02:
		getparm(param, 1);
		dp = save_string(dp, "%2d");
		pop();
		break;
	    case '3':
	      see03:
		getparm(param, 1);
		dp = save_string(dp, "%3d");
		pop();
		break;
	    case 'd':
		getparm(param, 1);
		dp = save_string(dp, "%d");
		pop();
		break;
	    case 'f':
		param++;
		break;
	    case 'b':
		param--;
		break;
	    case '\\':
		dp = save_string(dp, "%\\");
		break;
	    default:
	      invalid:
		dp = save_char(dp, '%');
		s--;
		_nc_warning("unknown %% code %s (%#x) in %s",
			    unctrl((chtype) *s), UChar(*s), cap);
		break;
	    }
	    break;
	default:
	    if (*s != '\0')
		dp = save_char(dp, *s++);
	    break;
	}
    }

    /*
     * Now, if we stripped off some leading padding, add it at the end
     * of the string as mandatory padding.
     */
    if (capstart) {
	dp = save_string(dp, "$<");
	for (s = capstart; *s != '\0'; s++)
	    if (isdigit(UChar(*s)) || *s == '*' || *s == '.')
		dp = save_char(dp, *s);
	    else
		break;
	dp = save_string(dp, "/>");
    }

    (void) save_char(dp, '\0');

    DEBUG_THIS(("... _nc_captoinfo %s", NonNull(my_string)));

    return (my_string);
}