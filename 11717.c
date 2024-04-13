tparm_trace_call(const char *string, TPARM_DATA * data)
{
    if (USE_TRACEF(TRACE_CALLS)) {
	int i;
	for (i = 0; i < data->num_actual; i++) {
	    if (data->p_is_s[i] != 0) {
		save_text(", %s", _nc_visbuf(data->p_is_s[i]), 0);
	    } else if ((long) data->param[i] > MAX_OF_TYPE(NCURSES_INT2) ||
		       (long) data->param[i] < 0) {
		_tracef("BUG: problem with tparm parameter #%d of %d",
			i + 1, data->num_actual);
		break;
	    } else {
		save_number(", %d", (int) data->param[i], 0);
	    }
	}
	_tracef(T_CALLED("%s(%s%s)"), TPS(tname), _nc_visbuf(string), TPS(out_buff));
	TPS(out_used) = 0;
	_nc_unlock_global(tracef);
    }
}