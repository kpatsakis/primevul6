nv_tilde(cmdarg_T *cap)
{
    if (!p_to && !VIsual_active && cap->oap->op_type != OP_TILDE)
    {
#ifdef FEAT_JOB_CHANNEL
	if (bt_prompt(curbuf) && !prompt_curpos_editable())
	{
	    clearopbeep(cap->oap);
	    return;
	}
#endif
	n_swapchar(cap);
    }
    else
	nv_operator(cap);
}