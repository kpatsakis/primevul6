cbuffer_process_args(
	exarg_T		*eap,
	buf_T		**bufp,
	linenr_T	*line1,
	linenr_T	*line2)
{
    buf_T	*buf = NULL;

    if (*eap->arg == NUL)
	buf = curbuf;
    else if (*skipwhite(skipdigits(eap->arg)) == NUL)
	buf = buflist_findnr(atoi((char *)eap->arg));

    if (buf == NULL)
    {
	emsg(_(e_invalid_argument));
	return FAIL;
    }

    if (buf->b_ml.ml_mfp == NULL)
    {
	emsg(_(e_buffer_is_not_loaded));
	return FAIL;
    }

    if (eap->addr_count == 0)
    {
	eap->line1 = 1;
	eap->line2 = buf->b_ml.ml_line_count;
    }

    if (eap->line1 < 1 || eap->line1 > buf->b_ml.ml_line_count
	    || eap->line2 < 1 || eap->line2 > buf->b_ml.ml_line_count)
    {
	emsg(_(e_invalid_range));
	return FAIL;
    }

    *line1 = eap->line1;
    *line2 = eap->line2;
    *bufp = buf;

    return OK;
}