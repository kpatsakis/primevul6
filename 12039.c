qf_parse_file_pfx(
	int idx,
	qffields_T *fields,
	qf_list_T *qfl,
	char_u *tail)
{
    fields->valid = FALSE;
    if (*fields->namebuf == NUL || mch_getperm(fields->namebuf) >= 0)
    {
	if (*fields->namebuf && idx == 'P')
	    qfl->qf_currfile =
		qf_push_dir(fields->namebuf, &qfl->qf_file_stack, TRUE);
	else if (idx == 'Q')
	    qfl->qf_currfile = qf_pop_dir(&qfl->qf_file_stack);
	*fields->namebuf = NUL;
	if (tail && *tail)
	{
	    STRMOVE(IObuff, skipwhite(tail));
	    qfl->qf_multiscan = TRUE;
	    return QF_MULTISCAN;
	}
    }

    return QF_OK;
}