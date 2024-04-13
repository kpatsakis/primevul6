nv_end(cmdarg_T *cap)
{
    if (cap->arg || (mod_mask & MOD_MASK_CTRL))	// CTRL-END = goto last line
    {
	cap->arg = TRUE;
	nv_goto(cap);
	cap->count1 = 1;		// to end of current line
    }
    nv_dollar(cap);
}