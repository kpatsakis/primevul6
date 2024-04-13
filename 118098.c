match_optname(user_string, opt_name, min_length, val_allowed)
const char *user_string, *opt_name;
int min_length;
boolean val_allowed;
{
	int len = (int)strlen(user_string);

	if (val_allowed) {
	    const char *p = index(user_string, ':'),
		       *q = index(user_string, '=');

	    if (!p || (q && q < p)) p = q;
	    while(p && p > user_string && isspace(*(p-1))) p--;
	    if (p) len = (int)(p - user_string);
	}

	return (len >= min_length) && !strncmpi(opt_name, user_string, len);
}