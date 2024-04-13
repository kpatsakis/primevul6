nv_halfpage(cmdarg_T *cap)
{
    if ((cap->cmdchar == Ctrl_U && curwin->w_cursor.lnum == 1)
	    || (cap->cmdchar == Ctrl_D
		&& curwin->w_cursor.lnum == curbuf->b_ml.ml_line_count))
	clearopbeep(cap->oap);
    else if (!checkclearop(cap->oap))
	halfpage(cap->cmdchar == Ctrl_D, cap->count0);
}