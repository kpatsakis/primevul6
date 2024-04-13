normal_cmd_get_more_chars(
	int	    idx_arg,
	cmdarg_T    *cap,
	int	    *need_flushbuf UNUSED)
{
    int		idx = idx_arg;
    int		c;
    int		*cp;
    int		repl = FALSE;	// get character for replace mode
    int		lit = FALSE;	// get extra character literally
    int		langmap_active = FALSE;    // using :lmap mappings
    int		lang;		// getting a text character
#ifdef HAVE_INPUT_METHOD
    int		save_smd;	// saved value of p_smd
#endif

    ++no_mapping;
    ++allow_keys;		// no mapping for nchar, but allow key codes
    // Don't generate a CursorHold event here, most commands can't handle
    // it, e.g., nv_replace(), nv_csearch().
    did_cursorhold = TRUE;
    if (cap->cmdchar == 'g')
    {
	/*
	 * For 'g' get the next character now, so that we can check for
	 * "gr", "g'" and "g`".
	 */
	cap->nchar = plain_vgetc();
	LANGMAP_ADJUST(cap->nchar, TRUE);
#ifdef FEAT_CMDL_INFO
	*need_flushbuf |= add_to_showcmd(cap->nchar);
#endif
	if (cap->nchar == 'r' || cap->nchar == '\'' || cap->nchar == '`'
		|| cap->nchar == Ctrl_BSL)
	{
	    cp = &cap->extra_char;	// need to get a third character
	    if (cap->nchar != 'r')
		lit = TRUE;			// get it literally
	    else
		repl = TRUE;		// get it in replace mode
	}
	else
	    cp = NULL;		// no third character needed
    }
    else
    {
	if (cap->cmdchar == 'r')		// get it in replace mode
	    repl = TRUE;
	cp = &cap->nchar;
    }
    lang = (repl || (nv_cmds[idx].cmd_flags & NV_LANG));

    /*
     * Get a second or third character.
     */
    if (cp != NULL)
    {
	if (repl)
	{
	    State = MODE_REPLACE;	// pretend Replace mode
#ifdef CURSOR_SHAPE
	    ui_cursor_shape();	// show different cursor shape
#endif
	}
	if (lang && curbuf->b_p_iminsert == B_IMODE_LMAP)
	{
	    // Allow mappings defined with ":lmap".
	    --no_mapping;
	    --allow_keys;
	    if (repl)
		State = MODE_LREPLACE;
	    else
		State = MODE_LANGMAP;
	    langmap_active = TRUE;
	}
#ifdef HAVE_INPUT_METHOD
	save_smd = p_smd;
	p_smd = FALSE;	// Don't let the IM code show the mode here
	if (lang && curbuf->b_p_iminsert == B_IMODE_IM)
	    im_set_active(TRUE);
#endif
	if ((State & MODE_INSERT) && !p_ek)
	{
#ifdef FEAT_JOB_CHANNEL
	    ch_log_output = TRUE;
#endif
	    // Disable bracketed paste and modifyOtherKeys here, we won't
	    // recognize the escape sequences with 'esckeys' off.
	    out_str(T_BD);
	    out_str(T_CTE);
	}

	*cp = plain_vgetc();

	if ((State & MODE_INSERT) && !p_ek)
	{
#ifdef FEAT_JOB_CHANNEL
	    ch_log_output = TRUE;
#endif
	    // Re-enable bracketed paste mode and modifyOtherKeys
	    out_str(T_BE);
	    out_str(T_CTI);
	}

	if (langmap_active)
	{
	    // Undo the decrement done above
	    ++no_mapping;
	    ++allow_keys;
	    State = MODE_NORMAL_BUSY;
	}
#ifdef HAVE_INPUT_METHOD
	if (lang)
	{
	    if (curbuf->b_p_iminsert != B_IMODE_LMAP)
		im_save_status(&curbuf->b_p_iminsert);
	    im_set_active(FALSE);
	}
	p_smd = save_smd;
#endif
	State = MODE_NORMAL_BUSY;
#ifdef FEAT_CMDL_INFO
	*need_flushbuf |= add_to_showcmd(*cp);
#endif

	if (!lit)
	{
#ifdef FEAT_DIGRAPHS
	    // Typing CTRL-K gets a digraph.
	    if (*cp == Ctrl_K
		    && ((nv_cmds[idx].cmd_flags & NV_LANG)
			|| cp == &cap->extra_char)
		    && vim_strchr(p_cpo, CPO_DIGRAPH) == NULL)
	    {
		c = get_digraph(FALSE);
		if (c > 0)
		{
		    *cp = c;
# ifdef FEAT_CMDL_INFO
		    // Guessing how to update showcmd here...
		    del_from_showcmd(3);
		    *need_flushbuf |= add_to_showcmd(*cp);
# endif
		}
	    }
#endif

	    // adjust chars > 127, except after "tTfFr" commands
	    LANGMAP_ADJUST(*cp, !lang);
#ifdef FEAT_RIGHTLEFT
	    // adjust Hebrew mapped char
	    if (p_hkmap && lang && KeyTyped)
		*cp = hkmap(*cp);
#endif
	}

	// When the next character is CTRL-\ a following CTRL-N means the
	// command is aborted and we go to Normal mode.
	if (cp == &cap->extra_char
		&& cap->nchar == Ctrl_BSL
		&& (cap->extra_char == Ctrl_N || cap->extra_char == Ctrl_G))
	{
	    cap->cmdchar = Ctrl_BSL;
	    cap->nchar = cap->extra_char;
	    idx = find_command(cap->cmdchar);
	}
	else if ((cap->nchar == 'n' || cap->nchar == 'N') && cap->cmdchar == 'g')
	    cap->oap->op_type = get_op_type(*cp, NUL);
	else if (*cp == Ctrl_BSL)
	{
	    long towait = (p_ttm >= 0 ? p_ttm : p_tm);

	    // There is a busy wait here when typing "f<C-\>" and then
	    // something different from CTRL-N.  Can't be avoided.
	    while ((c = vpeekc()) <= 0 && towait > 0L)
	    {
		do_sleep(towait > 50L ? 50L : towait, FALSE);
		towait -= 50L;
	    }
	    if (c > 0)
	    {
		c = plain_vgetc();
		if (c != Ctrl_N && c != Ctrl_G)
		    vungetc(c);
		else
		{
		    cap->cmdchar = Ctrl_BSL;
		    cap->nchar = c;
		    idx = find_command(cap->cmdchar);
		}
	    }
	}

	// When getting a text character and the next character is a
	// multi-byte character, it could be a composing character.
	// However, don't wait for it to arrive. Also, do enable mapping,
	// because if it's put back with vungetc() it's too late to apply
	// mapping.
	--no_mapping;
	while (enc_utf8 && lang && (c = vpeekc()) > 0
		&& (c >= 0x100 || MB_BYTE2LEN(vpeekc()) > 1))
	{
	    c = plain_vgetc();
	    if (!utf_iscomposing(c))
	    {
		vungetc(c);		// it wasn't, put it back
		break;
	    }
	    else if (cap->ncharC1 == 0)
		cap->ncharC1 = c;
	    else
		cap->ncharC2 = c;
	}
	++no_mapping;
    }
    --no_mapping;
    --allow_keys;

    return idx;
}