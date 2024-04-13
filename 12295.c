nv_optrans(cmdarg_T *cap)
{
    static char_u *(ar[8]) = {(char_u *)"dl", (char_u *)"dh",
			      (char_u *)"d$", (char_u *)"c$",
			      (char_u *)"cl", (char_u *)"cc",
			      (char_u *)"yy", (char_u *)":s\r"};
    static char_u *str = (char_u *)"xXDCsSY&";

    if (!checkclearopq(cap->oap))
    {
	// In Vi "2D" doesn't delete the next line.  Can't translate it
	// either, because "2." should also not use the count.
	if (cap->cmdchar == 'D' && vim_strchr(p_cpo, CPO_HASH) != NULL)
	{
	    cap->oap->start = curwin->w_cursor;
	    cap->oap->op_type = OP_DELETE;
#ifdef FEAT_EVAL
	    set_op_var(OP_DELETE);
#endif
	    cap->count1 = 1;
	    nv_dollar(cap);
	    finish_op = TRUE;
	    ResetRedobuff();
	    AppendCharToRedobuff('D');
	}
	else
	{
	    if (cap->count0)
		stuffnumReadbuff(cap->count0);
	    stuffReadbuff(ar[(int)(vim_strchr(str, cap->cmdchar) - str)]);
	}
    }
    cap->opcount = 0;
}