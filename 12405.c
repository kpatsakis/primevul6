nv_page(cmdarg_T *cap)
{
    if (!checkclearop(cap->oap))
    {
	if (mod_mask & MOD_MASK_CTRL)
	{
	    // <C-PageUp>: tab page back; <C-PageDown>: tab page forward
	    if (cap->arg == BACKWARD)
		goto_tabpage(-(int)cap->count1);
	    else
		goto_tabpage((int)cap->count0);
	}
	else
	    (void)onepage(cap->arg, cap->count1);
    }
}