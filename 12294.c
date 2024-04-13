v_visop(cmdarg_T *cap)
{
    static char_u trans[] = "YyDdCcxdXdAAIIrr";

    // Uppercase means linewise, except in block mode, then "D" deletes till
    // the end of the line, and "C" replaces till EOL
    if (isupper(cap->cmdchar))
    {
	if (VIsual_mode != Ctrl_V)
	{
	    VIsual_mode_orig = VIsual_mode;
	    VIsual_mode = 'V';
	}
	else if (cap->cmdchar == 'C' || cap->cmdchar == 'D')
	    curwin->w_curswant = MAXCOL;
    }
    cap->cmdchar = *(vim_strchr(trans, cap->cmdchar) + 1);
    nv_operator(cap);
}