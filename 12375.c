nv_kundo(cmdarg_T *cap)
{
    if (!checkclearopq(cap->oap))
    {
#ifdef FEAT_JOB_CHANNEL
	if (bt_prompt(curbuf))
	{
	    clearopbeep(cap->oap);
	    return;
	}
#endif
	u_undo((int)cap->count1);
	curwin->w_set_curswant = TRUE;
    }
}