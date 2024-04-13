nv_lineop(cmdarg_T *cap)
{
    cap->oap->motion_type = MLINE;
    if (cursor_down(cap->count1 - 1L, cap->oap->op_type == OP_NOP) == FAIL)
	clearopbeep(cap->oap);
    else if (  (cap->oap->op_type == OP_DELETE // only with linewise motions
		&& cap->oap->motion_force != 'v'
		&& cap->oap->motion_force != Ctrl_V)
	    || cap->oap->op_type == OP_LSHIFT
	    || cap->oap->op_type == OP_RSHIFT)
	beginline(BL_SOL | BL_FIX);
    else if (cap->oap->op_type != OP_YANK)	// 'Y' does not move cursor
	beginline(BL_WHITE | BL_FIX);
}