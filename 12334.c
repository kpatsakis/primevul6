nv_addsub(cmdarg_T *cap)
{
#ifdef FEAT_JOB_CHANNEL
    if (bt_prompt(curbuf) && !prompt_curpos_editable())
	clearopbeep(cap->oap);
    else
#endif
    if (!VIsual_active && cap->oap->op_type == OP_NOP)
    {
	prep_redo_cmd(cap);
	cap->oap->op_type = cap->cmdchar == Ctrl_A ?  OP_NR_ADD : OP_NR_SUB;
	op_addsub(cap->oap, cap->count1, cap->arg);
	cap->oap->op_type = OP_NOP;
    }
    else if (VIsual_active)
	nv_operator(cap);
    else
	clearop(cap->oap);
}