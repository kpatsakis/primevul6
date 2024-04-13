nv_bck_word(cmdarg_T *cap)
{
    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    curwin->w_set_curswant = TRUE;
    if (bck_word(cap->count1, cap->arg, FALSE) == FAIL)
	clearopbeep(cap->oap);
#ifdef FEAT_FOLDING
    else if ((fdo_flags & FDO_HOR) && KeyTyped && cap->oap->op_type == OP_NOP)
	foldOpenCursor();
#endif
}