normal_cmd_needs_more_chars(cmdarg_T *cap, short_u cmd_flags)
{
    return ((cmd_flags & NV_NCH)
	    && (((cmd_flags & NV_NCH_NOP) == NV_NCH_NOP
		    && cap->oap->op_type == OP_NOP)
		|| (cmd_flags & NV_NCH_ALW) == NV_NCH_ALW
		|| (cap->cmdchar == 'q'
		    && cap->oap->op_type == OP_NOP
		    && reg_recording == 0
		    && reg_executing == 0)
		|| ((cap->cmdchar == 'a' || cap->cmdchar == 'i')
		    && (cap->oap->op_type != OP_NOP || VIsual_active))));
}