nv_regname(cmdarg_T *cap)
{
    if (checkclearop(cap->oap))
	return;
#ifdef FEAT_EVAL
    if (cap->nchar == '=')
	cap->nchar = get_expr_register();
#endif
    if (cap->nchar != NUL && valid_yank_reg(cap->nchar, FALSE))
    {
	cap->oap->regname = cap->nchar;
	cap->opcount = cap->count0;	// remember count before '"'
#ifdef FEAT_EVAL
	set_reg_var(cap->oap->regname);
#endif
    }
    else
	clearopbeep(cap->oap);
}