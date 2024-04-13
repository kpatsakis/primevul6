get_number(NCURSES_CONST char *cap, int map)
{
    int result = map;
    if (cap != 0) {
	int check = tigetnum(cap);
	if (check > 0)
	    result = check;
    }
    return result;
}