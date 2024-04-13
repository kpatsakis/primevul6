nv_home(cmdarg_T *cap)
{
    // CTRL-HOME is like "gg"
    if (mod_mask & MOD_MASK_CTRL)
	nv_goto(cap);
    else
    {
	cap->count0 = 1;
	nv_pipe(cap);
    }
    ins_at_eol = FALSE;	    // Don't move cursor past eol (only necessary in a
			    // one-character line).
}