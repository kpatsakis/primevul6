nv_zg_zw(cmdarg_T *cap, int nchar)
{
    char_u	*ptr = NULL;
    int		len;
    int		undo = FALSE;

    if (nchar == 'u')
    {
	++no_mapping;
	++allow_keys;   // no mapping for nchar, but allow key codes
	nchar = plain_vgetc();
	LANGMAP_ADJUST(nchar, TRUE);
	--no_mapping;
	--allow_keys;
#ifdef FEAT_CMDL_INFO
	(void)add_to_showcmd(nchar);
#endif
	if (vim_strchr((char_u *)"gGwW", nchar) == NULL)
	{
	    clearopbeep(cap->oap);
	    return OK;
	}
	undo = TRUE;
    }

    if (checkclearop(cap->oap))
	return OK;
    if (VIsual_active && get_visual_text(cap, &ptr, &len) == FAIL)
	return FAIL;
    if (ptr == NULL)
    {
	pos_T	pos = curwin->w_cursor;

	// Find bad word under the cursor.  When 'spell' is
	// off this fails and find_ident_under_cursor() is
	// used below.
	emsg_off++;
	len = spell_move_to(curwin, FORWARD, TRUE, TRUE, NULL);
	emsg_off--;
	if (len != 0 && curwin->w_cursor.col <= pos.col)
	    ptr = ml_get_pos(&curwin->w_cursor);
	curwin->w_cursor = pos;
    }

    if (ptr == NULL
		&& (len = find_ident_under_cursor(&ptr, FIND_IDENT)) == 0)
	return FAIL;
    spell_add_word(ptr, len, nchar == 'w' || nchar == 'W'
	    ? SPELL_ADD_BAD : SPELL_ADD_GOOD,
	    (nchar == 'G' || nchar == 'W') ? 0 : (int)cap->count1, undo);

    return OK;
}