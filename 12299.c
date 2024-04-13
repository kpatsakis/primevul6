nv_record(cmdarg_T *cap)
{
    if (cap->oap->op_type == OP_FORMAT)
    {
	// "gqq" is the same as "gqgq": format line
	cap->cmdchar = 'g';
	cap->nchar = 'q';
	nv_operator(cap);
    }
    else if (!checkclearop(cap->oap))
    {
#ifdef FEAT_CMDWIN
	if (cap->nchar == ':' || cap->nchar == '/' || cap->nchar == '?')
	{
	    stuffcharReadbuff(cap->nchar);
	    stuffcharReadbuff(K_CMDWIN);
	}
	else
#endif
	    // (stop) recording into a named register, unless executing a
	    // register
	    if (reg_executing == 0 && do_record(cap->nchar) == FAIL)
		clearopbeep(cap->oap);
    }
}