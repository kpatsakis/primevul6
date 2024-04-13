nv_operator(cmdarg_T *cap)
{
    int	    op_type;

    op_type = get_op_type(cap->cmdchar, cap->nchar);
#ifdef FEAT_JOB_CHANNEL
    if (bt_prompt(curbuf) && op_is_change(op_type) && !prompt_curpos_editable())
    {
	clearopbeep(cap->oap);
	return;
    }
#endif

    if (op_type == cap->oap->op_type)	    // double operator works on lines
	nv_lineop(cap);
    else if (!checkclearop(cap->oap))
    {
	cap->oap->start = curwin->w_cursor;
	cap->oap->op_type = op_type;
#ifdef FEAT_EVAL
	set_op_var(op_type);
#endif
    }
}