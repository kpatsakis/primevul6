nv_pipe(cmdarg_T *cap)
{
    cap->oap->motion_type = MCHAR;
    cap->oap->inclusive = FALSE;
    beginline(0);
    if (cap->count0 > 0)
    {
	coladvance((colnr_T)(cap->count0 - 1));
	curwin->w_curswant = (colnr_T)(cap->count0 - 1);
    }
    else
	curwin->w_curswant = 0;
    // keep curswant at the column where we wanted to go, not where
    // we ended; differs if line is too short
    curwin->w_set_curswant = FALSE;
}