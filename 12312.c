nv_up(cmdarg_T *cap)
{
    if (mod_mask & MOD_MASK_SHIFT)
    {
	// <S-Up> is page up
	cap->arg = BACKWARD;
	nv_page(cap);
    }
    else
    {
	cap->oap->motion_type = MLINE;
	if (cursor_up(cap->count1, cap->oap->op_type == OP_NOP) == FAIL)
	    clearopbeep(cap->oap);
	else if (cap->arg)
	    beginline(BL_WHITE | BL_FIX);
    }
}