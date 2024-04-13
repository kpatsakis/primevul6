nv_redo_or_register(cmdarg_T *cap)
{
    if (VIsual_select && VIsual_active)
    {
	int reg;
	// Get register name
	++no_mapping;
	++allow_keys;
	reg = plain_vgetc();
	LANGMAP_ADJUST(reg, TRUE);
	--no_mapping;
	--allow_keys;

	if (reg == '"')
	    // the unnamed register is 0
	    reg = 0;

	VIsual_select_reg = valid_yank_reg(reg, TRUE) ? reg : 0;
	return;
    }

    if (!checkclearopq(cap->oap))
    {
	u_redo((int)cap->count1);
	curwin->w_set_curswant = TRUE;
    }
}