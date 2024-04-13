msg_prt_line(char_u *s, int list)
{
    int		c;
    int		col = 0;
    int		n_extra = 0;
    int		c_extra = 0;
    int		c_final = 0;
    char_u	*p_extra = NULL;	    // init to make SASC shut up
    int		n;
    int		attr = 0;
    char_u	*trail = NULL;
    char_u	*lead = NULL;
    int		in_multispace = FALSE;
    int		multispace_pos = 0;
    int		l;
    char_u	buf[MB_MAXBYTES + 1];

    if (curwin->w_p_list)
	list = TRUE;

    if (list)
    {
	// find start of trailing whitespace
	if (curwin->w_lcs_chars.trail)
	{
	    trail = s + STRLEN(s);
	    while (trail > s && VIM_ISWHITE(trail[-1]))
		--trail;
	}
	// find end of leading whitespace
	if (curwin->w_lcs_chars.lead || curwin->w_lcs_chars.leadmultispace != NULL)
	{
	    lead = s;
	    while (VIM_ISWHITE(lead[0]))
		lead++;
	    // in a line full of spaces all of them are treated as trailing
	    if (*lead == NUL)
		lead = NULL;
	}
    }

    // output a space for an empty line, otherwise the line will be
    // overwritten
    if (*s == NUL && !(list && curwin->w_lcs_chars.eol != NUL))
	msg_putchar(' ');

    while (!got_int)
    {
	if (n_extra > 0)
	{
	    --n_extra;
	    if (n_extra == 0 && c_final)
		c = c_final;
	    else if (c_extra)
		c = c_extra;
	    else
		c = *p_extra++;
	}
	else if (has_mbyte && (l = (*mb_ptr2len)(s)) > 1)
	{
	    col += (*mb_ptr2cells)(s);
	    if (l >= MB_MAXBYTES)
	    {
		STRCPY(buf, "?");
	    }
	    else if (curwin->w_lcs_chars.nbsp != NUL && list
		    && (mb_ptr2char(s) == 160
			|| mb_ptr2char(s) == 0x202f))
	    {
		int len = mb_char2bytes(curwin->w_lcs_chars.nbsp, buf);

		buf[len] = NUL;
	    }
	    else
	    {
		mch_memmove(buf, s, (size_t)l);
		buf[l] = NUL;
	    }
	    msg_puts((char *)buf);
	    s += l;
	    continue;
	}
	else
	{
	    attr = 0;
	    c = *s++;
	    in_multispace = c == ' '
		&& ((col > 0 && s[-2] == ' ') || *s == ' ');
	    if (!in_multispace)
		multispace_pos = 0;
	    if (c == TAB && (!list || curwin->w_lcs_chars.tab1))
	    {
		// tab amount depends on current column
#ifdef FEAT_VARTABS
		n_extra = tabstop_padding(col, curbuf->b_p_ts,
						    curbuf->b_p_vts_array) - 1;
#else
		n_extra = curbuf->b_p_ts - col % curbuf->b_p_ts - 1;
#endif
		if (!list)
		{
		    c = ' ';
		    c_extra = ' ';
		    c_final = NUL;
		}
		else
		{
		    c = (n_extra == 0 && curwin->w_lcs_chars.tab3)
						? curwin->w_lcs_chars.tab3
						: curwin->w_lcs_chars.tab1;
		    c_extra = curwin->w_lcs_chars.tab2;
		    c_final = curwin->w_lcs_chars.tab3;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	    else if (c == 160 && list && curwin->w_lcs_chars.nbsp != NUL)
	    {
		c = curwin->w_lcs_chars.nbsp;
		attr = HL_ATTR(HLF_8);
	    }
	    else if (c == NUL && list && curwin->w_lcs_chars.eol != NUL)
	    {
		p_extra = (char_u *)"";
		c_extra = NUL;
		c_final = NUL;
		n_extra = 1;
		c = curwin->w_lcs_chars.eol;
		attr = HL_ATTR(HLF_AT);
		--s;
	    }
	    else if (c != NUL && (n = byte2cells(c)) > 1)
	    {
		n_extra = n - 1;
		p_extra = transchar_byte(c);
		c_extra = NUL;
		c_final = NUL;
		c = *p_extra++;
		// Use special coloring to be able to distinguish <hex> from
		// the same in plain text.
		attr = HL_ATTR(HLF_8);
	    }
	    else if (c == ' ')
	    {
		if (list && lead != NULL && s <= lead && in_multispace
			&& curwin->w_lcs_chars.leadmultispace != NULL)
		{
		    c = curwin->w_lcs_chars.leadmultispace[multispace_pos++];
		    if (curwin->w_lcs_chars.leadmultispace[multispace_pos]
									== NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		else if (lead != NULL && s <= lead
					    && curwin->w_lcs_chars.lead != NUL)
		{
		    c = curwin->w_lcs_chars.lead;
		    attr = HL_ATTR(HLF_8);
		}
		else if (trail != NULL && s > trail)
		{
		    c = curwin->w_lcs_chars.trail;
		    attr = HL_ATTR(HLF_8);
		}
		else if (list && in_multispace
			&& curwin->w_lcs_chars.multispace != NULL)
		{
		    c = curwin->w_lcs_chars.multispace[multispace_pos++];
		    if (curwin->w_lcs_chars.multispace[multispace_pos] == NUL)
			multispace_pos = 0;
		    attr = HL_ATTR(HLF_8);
		}
		else if (list && curwin->w_lcs_chars.space != NUL)
		{
		    c = curwin->w_lcs_chars.space;
		    attr = HL_ATTR(HLF_8);
		}
	    }
	}

	if (c == NUL)
	    break;

	msg_putchar_attr(c, attr);
	col++;
    }
    msg_clr_eos();
}