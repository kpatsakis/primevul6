show_where(unsigned level)
{
    if (_nc_tracing >= DEBUG_LEVEL(level)) {
	char my_name[MAX_NAME_SIZE];
	_nc_get_type(my_name);
	_tracef("\"%s\", line %d, '%s'",
		_nc_get_source(),
		_nc_curr_line, my_name);
    }
}