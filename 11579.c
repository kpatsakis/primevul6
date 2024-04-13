get_number(NCURSES_CONST char *cap, const char *env)
{
    int result = tgetnum(cap);
    char *value = env ? getenv(env) : 0;
    if (value != 0 && *value != 0) {
	char *next = 0;
	long check = strtol(value, &next, 10);
	if (check > 0 && *next == '\0')
	    result = (int) check;
    }
    return result;
}