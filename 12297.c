nv_z_get_count(cmdarg_T *cap, int *nchar_arg)
{
    int		nchar = *nchar_arg;
    long	n;

    // "z123{nchar}": edit the count before obtaining {nchar}
    if (checkclearop(cap->oap))
	return FALSE;
    n = nchar - '0';

    for (;;)
    {
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = TRUE;		// disallow scrolling here
#endif
	++no_mapping;
	++allow_keys;   // no mapping for nchar, but allow key codes
	nchar = plain_vgetc();
	LANGMAP_ADJUST(nchar, TRUE);
	--no_mapping;
	--allow_keys;
#ifdef FEAT_CMDL_INFO
	(void)add_to_showcmd(nchar);
#endif
	if (nchar == K_DEL || nchar == K_KDEL)
	    n /= 10;
	else if (VIM_ISDIGIT(nchar))
	    n = n * 10 + (nchar - '0');
	else if (nchar == CAR)
	{
#ifdef FEAT_GUI
	    need_mouse_correct = TRUE;
#endif
	    win_setheight((int)n);
	    break;
	}
	else if (nchar == 'l'
		|| nchar == 'h'
		|| nchar == K_LEFT
		|| nchar == K_RIGHT)
	{
	    cap->count1 = n ? n * cap->count1 : cap->count1;
	    *nchar_arg = nchar;
	    return TRUE;
	}
	else
	{
	    clearopbeep(cap->oap);
	    break;
	}
    }
    cap->oap->op_type = OP_NOP;
    return FALSE;
}