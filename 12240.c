qf_parse_dir_pfx(int idx, qffields_T *fields, qf_list_T *qfl)
{
    if (idx == 'D')				// enter directory
    {
	if (*fields->namebuf == NUL)
	{
	    emsg(_(e_missing_or_empty_directory_name));
	    return QF_FAIL;
	}
	qfl->qf_directory =
	    qf_push_dir(fields->namebuf, &qfl->qf_dir_stack, FALSE);
	if (qfl->qf_directory == NULL)
	    return QF_FAIL;
    }
    else if (idx == 'X')			// leave directory
	qfl->qf_directory = qf_pop_dir(&qfl->qf_dir_stack);

    return QF_OK;
}