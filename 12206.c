qf_get_nextline(qfstate_T *state)
{
    int status = QF_FAIL;

    if (state->fd == NULL)
    {
	if (state->tv != NULL)
	{
	    if (state->tv->v_type == VAR_STRING)
		// Get the next line from the supplied string
		status = qf_get_next_str_line(state);
	    else if (state->tv->v_type == VAR_LIST)
		// Get the next line from the supplied list
		status = qf_get_next_list_line(state);
	}
	else
	    // Get the next line from the supplied buffer
	    status = qf_get_next_buf_line(state);
    }
    else
	// Get the next line from the supplied file
	status = qf_get_next_file_line(state);

    if (status != QF_OK)
	return status;

    // remove newline/CR from the line
    if (state->linelen > 0 && state->linebuf[state->linelen - 1] == '\n')
    {
	state->linebuf[state->linelen - 1] = NUL;
#ifdef USE_CRNL
	if (state->linelen > 1 && state->linebuf[state->linelen - 2] == '\r')
	    state->linebuf[state->linelen - 2] = NUL;
#endif
    }

    remove_bom(state->linebuf);

    return QF_OK;
}