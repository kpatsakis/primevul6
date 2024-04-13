check_user_capability_type(const char *name, int actual)
{
    if (lookup_user_capability(name) == 0) {
	int expected = standard_type(name);
	if (expected >= 0) {
	    _nc_warning("expected %s to be %s, but actually %s",
			name,
			name_of_type(actual),
			name_of_type(expected)
		);
	} else if (*name != 'k') {
	    _nc_warning("undocumented %s capability %s",
			name_of_type(actual),
			name);
	}
    }
}