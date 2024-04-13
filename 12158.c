qf_add_entry_from_dict(
	qf_list_T	*qfl,
	dict_T		*d,
	int		first_entry,
	int		*valid_entry)
{
    static int	did_bufnr_emsg;
    char_u	*filename, *module, *pattern, *text, *type;
    int		bufnum, valid, status, col, end_col, vcol, nr;
    long	lnum, end_lnum;

    if (first_entry)
	did_bufnr_emsg = FALSE;

    filename = dict_get_string(d, "filename", TRUE);
    module = dict_get_string(d, "module", TRUE);
    bufnum = (int)dict_get_number(d, "bufnr");
    lnum = (int)dict_get_number(d, "lnum");
    end_lnum = (int)dict_get_number(d, "end_lnum");
    col = (int)dict_get_number(d, "col");
    end_col = (int)dict_get_number(d, "end_col");
    vcol = (int)dict_get_number(d, "vcol");
    nr = (int)dict_get_number(d, "nr");
    type = dict_get_string(d, "type", TRUE);
    pattern = dict_get_string(d, "pattern", TRUE);
    text = dict_get_string(d, "text", TRUE);
    if (text == NULL)
	text = vim_strsave((char_u *)"");

    valid = TRUE;
    if ((filename == NULL && bufnum == 0) || (lnum == 0 && pattern == NULL))
	valid = FALSE;

    // Mark entries with non-existing buffer number as not valid. Give the
    // error message only once.
    if (bufnum != 0 && (buflist_findnr(bufnum) == NULL))
    {
	if (!did_bufnr_emsg)
	{
	    did_bufnr_emsg = TRUE;
	    semsg(_(e_buffer_nr_not_found), bufnum);
	}
	valid = FALSE;
	bufnum = 0;
    }

    // If the 'valid' field is present it overrules the detected value.
    if (dict_has_key(d, "valid"))
	valid = (int)dict_get_bool(d, "valid", FALSE);

    status =  qf_add_entry(qfl,
			NULL,		// dir
			filename,
			module,
			bufnum,
			text,
			lnum,
			end_lnum,
			col,
			end_col,
			vcol,		// vis_col
			pattern,	// search pattern
			nr,
			type == NULL ? NUL : *type,
			valid);

    vim_free(filename);
    vim_free(module);
    vim_free(pattern);
    vim_free(text);
    vim_free(type);

    if (valid)
	*valid_entry = TRUE;

    return status;
}