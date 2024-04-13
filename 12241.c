qf_init_process_nextline(
	qf_list_T	*qfl,
	efm_T		*fmt_first,
	qfstate_T	*state,
	qffields_T	*fields)
{
    int		    status;

    // Get the next line from a file/buffer/list/string
    status = qf_get_nextline(state);
    if (status != QF_OK)
	return status;

    status = qf_parse_line(qfl, state->linebuf, state->linelen,
	    fmt_first, fields);
    if (status != QF_OK)
	return status;

    return qf_add_entry(qfl,
		qfl->qf_directory,
		(*fields->namebuf || qfl->qf_directory != NULL)
		? fields->namebuf
		: ((qfl->qf_currfile != NULL && fields->valid)
		    ? qfl->qf_currfile : (char_u *)NULL),
		fields->module,
		0,
		fields->errmsg,
		fields->lnum,
		fields->end_lnum,
		fields->col,
		fields->end_col,
		fields->use_viscol,
		fields->pattern,
		fields->enr,
		fields->type,
		fields->valid);
}