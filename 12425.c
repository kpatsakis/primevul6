nv_subst(cmdarg_T *cap)
{
#ifdef FEAT_TERMINAL
    // When showing output of term_dumpdiff() swap the top and bottom.
    if (term_swap_diff() == OK)
	return;
#endif
#ifdef FEAT_JOB_CHANNEL
    if (bt_prompt(curbuf) && !prompt_curpos_editable())
    {
	clearopbeep(cap->oap);
	return;
    }
#endif
    if (VIsual_active)	// "vs" and "vS" are the same as "vc"
    {
	if (cap->cmdchar == 'S')
	{
	    VIsual_mode_orig = VIsual_mode;
	    VIsual_mode = 'V';
	}
	cap->cmdchar = 'c';
	nv_operator(cap);
    }
    else
	nv_optrans(cap);
}