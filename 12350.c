nv_brace(cmdarg_T *cap)
{
    cap->oap->motion_type = MCHAR;
    cap->oap->use_reg_one = TRUE;
    // The motion used to be inclusive for "(", but that is not what Vi does.
    cap->oap->inclusive = FALSE;
    curwin->w_set_curswant = TRUE;

    if (findsent(cap->arg, cap->count1) == FAIL)
	clearopbeep(cap->oap);
    else
    {
	// Don't leave the cursor on the NUL past end of line.
	adjust_cursor(cap->oap);
	curwin->w_cursor.coladd = 0;
#ifdef FEAT_FOLDING
	if ((fdo_flags & FDO_BLOCK) && KeyTyped && cap->oap->op_type == OP_NOP)
	    foldOpenCursor();
#endif
    }
}