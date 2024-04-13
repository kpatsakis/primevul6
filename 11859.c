save_string(char *d, const char *const s)
{
    size_t have = (size_t) (d - my_string);
    size_t need = have + strlen(s) + 2;
    if (need > my_length) {
	my_string = (char *) _nc_doalloc(my_string, my_length = (need + need));
	if (my_string == 0)
	    _nc_err_abort(MSG_NO_MEMORY);
	d = my_string + have;
    }
    _nc_STRCPY(d, s, my_length - have);
    return d + strlen(d);
}