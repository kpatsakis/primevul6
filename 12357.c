nv_findpar(cmdarg_T *cap)
{
    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    cap->oap->use_reg_one = TRUE;
    curwin->w_set_curswant = TRUE;
    if (!findpar(&cap->oap->inclusive, cap->arg, cap->count1, NUL, FALSE))
	clearopbeep(cap->oap);
    else
    {
	curwin->w_cursor.coladd = 0;
#ifdef FEAT_FOLDING
	if ((fdo_flags & FDO_BLOCK) && KeyTyped && cap->oap->op_type == OP_NOP)
	    foldOpenCursor();
#endif
    }
}