nv_at(cmdarg_T *cap)
{
    if (checkclearop(cap->oap))
	return;
#ifdef FEAT_EVAL
    if (cap->nchar == '=')
    {
	if (get_expr_register() == NUL)
	    return;
    }
#endif
    while (cap->count1-- && !got_int)
    {
	if (do_execreg(cap->nchar, FALSE, FALSE, FALSE) == FAIL)
	{
	    clearopbeep(cap->oap);
	    break;
	}
	line_breakcheck();
    }
}