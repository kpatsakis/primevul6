nv_join(cmdarg_T *cap)
{
    if (VIsual_active)	// join the visual lines
	nv_operator(cap);
    else if (!checkclearop(cap->oap))
    {
	if (cap->count0 <= 1)
	    cap->count0 = 2;	    // default for join is two lines!
	if (curwin->w_cursor.lnum + cap->count0 - 1 >
						   curbuf->b_ml.ml_line_count)
	{
	    // can't join when on the last line
	    if (cap->count0 <= 2)
	    {
		clearopbeep(cap->oap);
		return;
	    }
	    cap->count0 = curbuf->b_ml.ml_line_count
						  - curwin->w_cursor.lnum + 1;
	}

	prep_redo(cap->oap->regname, cap->count0,
				     NUL, cap->cmdchar, NUL, NUL, cap->nchar);
	(void)do_join(cap->count0, cap->nchar == NUL, TRUE, TRUE, TRUE);
    }
}