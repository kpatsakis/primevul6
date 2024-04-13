nv_beginline(cmdarg_T *cap)
{
    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    beginline(cap->arg);
#ifdef FEAT_FOLDING
    if ((fdo_flags & FDO_HOR) && KeyTyped && cap->oap->op_type == OP_NOP)
	foldOpenCursor();
#endif
    ins_at_eol = FALSE;	    // Don't move cursor past eol (only necessary in a
			    // one-character line).
}