nv_vreplace(cmdarg_T *cap)
{
    if (VIsual_active)
    {
	cap->cmdchar = 'r';
	cap->nchar = cap->extra_char;
	nv_replace(cap);	// Do same as "r" in Visual mode for now
    }
    else if (!checkclearopq(cap->oap))
    {
	if (!curbuf->b_p_ma)
	    emsg(_(e_cannot_make_changes_modifiable_is_off));
	else
	{
	    if (cap->extra_char == Ctrl_V)	// get another character
		cap->extra_char = get_literal(FALSE);
	    stuffcharReadbuff(cap->extra_char);
	    stuffcharReadbuff(ESC);
	    if (virtual_active())
		coladvance(getviscol());
	    invoke_edit(cap, TRUE, 'v', FALSE);
	}
    }
}