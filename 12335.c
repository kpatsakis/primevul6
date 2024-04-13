nv_Replace(cmdarg_T *cap)
{
    if (VIsual_active)		// "R" is replace lines
    {
	cap->cmdchar = 'c';
	cap->nchar = NUL;
	VIsual_mode_orig = VIsual_mode; // remember original area for gv
	VIsual_mode = 'V';
	nv_operator(cap);
    }
    else if (!checkclearopq(cap->oap))
    {
	if (!curbuf->b_p_ma)
	    emsg(_(e_cannot_make_changes_modifiable_is_off));
	else
	{
	    if (virtual_active())
		coladvance(getviscol());
	    invoke_edit(cap, FALSE, cap->arg ? 'V' : 'R', FALSE);
	}
    }
}